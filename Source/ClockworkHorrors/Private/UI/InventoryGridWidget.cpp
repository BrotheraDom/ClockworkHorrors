// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/InventoryGridWidget.h"
#include "Components/WrapBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "UI/InventorySlotWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"
#include "UI/MainInventoryWidget.h"
#include "BaseCharacter.h"
#include "Utils/InventoryComponent.h"

UInventoryGridWidget::UInventoryGridWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InventorySlotWidgets = TArray<UInventorySlotWidget*>();
}

void UInventoryGridWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UInventoryGridWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MaxSlotCount = 1;
}

void UInventoryGridWidget::EnsureSlotCount(int32 DesiredCount)
{
	if (!InventoryVerticalBox || !InventorySlotWidgetClass || bIsFixed)
	{
		return;
	}

	DesiredCount = FMath::Clamp(DesiredCount, 0, MaxSlotCount);

	while (InventorySlotWidgets.Num() < DesiredCount)
	{
		const int32 NewIndex = InventorySlotWidgets.Num();
		UHorizontalBox* TargetRow = GetOrCreateRowForIndex(NewIndex);

		UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, InventorySlotWidgetClass);
		if (!NewSlot)
		{
			break;
		}

		NewSlot->SetSlotIndex(NewIndex);

		if (MainInventoryReference)
		{
			NewSlot->OnPressEvent.AddDynamic(MainInventoryReference, &UMainInventoryWidget::HandleSlotPressed);
			NewSlot->OnDragStarted.AddDynamic(MainInventoryReference, &UMainInventoryWidget::HandleDragStarted);

			UWorld* World = GetWorld();
			if(!World)
			{
				UE_LOG(LogTemp, Error, TEXT("UInventoryGridWidget::EnsureSlotCount: World is null!"));
				return;
			}
			APlayerController* const PC = World->GetFirstPlayerController();
			if(!PC)
			{
				UE_LOG(LogTemp, Error, TEXT("UInventoryGridWidget::EnsureSlotCount: PlayerController is null!"));
				return;
			}
			ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(PC->GetPawn());
			if(!PlayerCharacter)
			{
				UE_LOG(LogTemp, Error, TEXT("UInventoryGridWidget::EnsureSlotCount: PlayerCharacter is null!"));
				return;
			}

			UInventoryComponent* InventoryComponent = PlayerCharacter->GetComponentByClass<UInventoryComponent>();
			if(!InventoryComponent)
			{
				UE_LOG(LogTemp, Error, TEXT("UInventoryGridWidget::EnsureSlotCount: InventoryComponent is null!"));
				return;
			}
			NewSlot->OnSwapEvent.AddDynamic(InventoryComponent, &UInventoryComponent::HandleSwapEvent);

		}

		UHorizontalBoxSlot* HBoxSlot = TargetRow->AddChildToHorizontalBox(NewSlot);
		if (HBoxSlot)
		{
			HBoxSlot->SetHorizontalAlignment(HAlign_Center);
			HBoxSlot->SetVerticalAlignment(VAlign_Center);
		}

		InventorySlotWidgets.Add(NewSlot);
	}
}

UHorizontalBox* UInventoryGridWidget::GetOrCreateRowForIndex(int32 SlotIndex)
{
	const int32 RowIndex = SlotIndex / MaxRow;

	if (InventoryRows.IsValidIndex(RowIndex))
	{
		return InventoryRows[RowIndex];
	}

	UHorizontalBox* NewRow = NewObject<UHorizontalBox>(this);
	UVerticalBoxSlot* VBoxSlot = InventoryVerticalBox->AddChildToVerticalBox(NewRow);
	VBoxSlot->SetHorizontalAlignment(HAlign_Center);

	InventoryRows.Add(NewRow);
	return NewRow;
}

void UInventoryGridWidget::AddItemDataToSlot(const FInventorySlotEntry& ItemData, int32 SlotIndex, bool ItemSwaped)
{
	UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: Adding Item Data to Slot Index %d: %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);

	if (!ItemData.IsValidEntry())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: ItemData is not valid! Cannot add item %s with Quantity %d to Slot Index %d"), *ItemData.GetItemDataName().ToString(), ItemData.Quantity, SlotIndex);
		return;
	}

	// If the item is not a swapped item, add it to the specified slot index
	if (!ItemSwaped)
	{
		if (!InventorySlotWidgets.IsValidIndex(SlotIndex))
		{
			if (CanIncreaseSlotCount(InventorySlotWidgets.Num() + 1)) // Check if we can Create a new slot if the specified index is out of bounds
			{
				//UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: Slot Index %d is out of bounds! Creating a new slot for item %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);
				EnsureSlotCount(InventorySlotWidgets.Num() + 1); // Try to add a new slot if we haven't reached the max slot count
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: Cannot Create New Slot! Max Slot Count %d reached"), MaxSlotCount);
				return;
			}
		}

		UInventorySlotWidget* TargetSlot = InventorySlotWidgets[SlotIndex];

		if (!TargetSlot)
		{
			UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: Target Slot %d is null! Cannot add item %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);
			return;
		}

		if (TargetSlot->IsSlotValid() && TargetSlot->CompareItemData(ItemData)) // If the slot already has an item and is the same item, add the quantity to the slot
		{
			TargetSlot->AddQuantityToSlot(ItemData.Quantity);
		}
		else if (!TargetSlot->IsSlotValid()) // If the slot is empty, add the item data to the slot
		{
			TargetSlot->AddItemDataToSlot(ItemData);
			ItemNameToSlotWidgetMap.Add(SlotIndex, TargetSlot);
		}

		InventorySlotWidgets[SlotIndex]->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		//UE_LOG(Game, Warning, TEXT("UInventoryGrid: Swapping Items: %s with Quantity %d to Slot Index %d"), *ItemName.ToString(), Quantity, SlotIndex);
		if (InventorySlotWidgets.IsValidIndex(SlotIndex))
		{
			UInventorySlotWidget* TargetSlot = InventorySlotWidgets[SlotIndex];
			if (TargetSlot)
			{
				TargetSlot->ClearSlot();
				if (!TargetSlot->IsSlotValid())
				{
					TargetSlot->AddItemDataToSlot(ItemData);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: Target Slot %d is null! Cannot add item %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: Slot Index %d is out of bounds! Cannot add item %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);
		}
	}
}

void UInventoryGridWidget::RemoveItemFromSlot(FName ItemName, int32 Quantity, int32 SlotIndex)
{
	if(!InventorySlotWidgets[SlotIndex])
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: Slot Index %d is out of bounds! Cannot remove item %s with Quantity %d"), SlotIndex, *ItemName.ToString(), Quantity);
		return;
	}

	InventorySlotWidgets[SlotIndex]->ClearSlot();
	InventorySlotWidgets[SlotIndex]->SetVisibility(ESlateVisibility::Hidden);
	ItemNameToSlotWidgetMap.Remove(SlotIndex);
}

bool UInventoryGridWidget::CanIncreaseSlotCount(int32 NewSlotCount) const
{
	if(bIsFixed)
	{
		return false;
	}

	return NewSlotCount <= MaxSlotCount;
}

void UInventoryGridWidget::SetInventoryMaxSlotCount(int32 NewMaxSlotCount)
{
	MaxSlotCount = NewMaxSlotCount;
}
