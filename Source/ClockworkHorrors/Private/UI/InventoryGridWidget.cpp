// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/InventoryGridWidget.h"
#include "Components/WrapBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "UI/InventorySlotWidget.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"

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
	//UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: Adding Item Data to Slot Index %d: %s with Quantity %d"), SlotIndex, *ItemData.GetItemDataName().ToString(), ItemData.Quantity);

	if (!ItemData.IsValidEntry())
	{
		//UE_LOG(Game, Warning, TEXT("UInventoryGrid: ItemData is not valid! Cannot add item %s with Quantity %d to Slot Index %d"), *ItemData.ItemName.ToString(), ItemData.Quantity, SlotIndex);
		return;
	}

	// If the item is not a swapped item, add it to the specified slot index
	if (!ItemSwaped)
	{
		if (!InventorySlotWidgets.IsValidIndex(SlotIndex))
		{
			UE_LOG(LogTemp, Error, TEXT("UInventoryGrid: Slot Index %d is out of bounds!"), SlotIndex);
			UE_LOG(LogTemp, Warning, TEXT("UInventoryGrid: Cannot add item %s with Quantity %d to Slot Index %d"), *ItemData.GetItemDataName().ToString(), ItemData.Quantity, SlotIndex);
			if(CanIncreaseSlotCount(InventorySlotWidgets.Num() + 1))
			{
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
			ItemNameToSlotWidgetMap.Add(ItemData.GetItemDataName(), TargetSlot);
		}
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
