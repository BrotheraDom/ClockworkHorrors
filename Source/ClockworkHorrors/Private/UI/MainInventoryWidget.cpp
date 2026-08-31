// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/MainInventoryWidget.h"
#include "BaseCharacter.h"
#include "UI/InventoryGridWidget.h"
#include "UI/ItemActionWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/SlateBlueprintLibrary.h"

UMainInventoryWidget::UMainInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMainInventoryWidget::NativeConstruct()
{
	if(InventoryGrid)
	{
		InventoryGrid->SetInventoryMainReference(this);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UMainInventoryWidget::NativeConstruct InventoryGrid is not initialized!"));
	}
	ItemActionWidget->SetVisibility(ESlateVisibility::Hidden);
}

void UMainInventoryWidget::ShowInventory(bool bInventoryShown)
{
	UWorld* World = GetWorld();

	if (World)
	{
		UGameInstance* GameInstance = GetGameInstance();

		if (GameInstance)
		{
			UE_LOG(LogTemp, Log, TEXT("UMainInventoryWidget::ShowInventory GameInstance found: %s"), *GameInstance->GetName());
			APlayerController* const PC = GameInstance ? GameInstance->GetFirstLocalPlayerController() : nullptr;
			if(!PC)
			{
				UE_LOG(LogTemp, Error, TEXT("UMainInventoryWidget::ShowInventory PlayerController not found!"));
				return;
			}
			if (bInventoryShown)
			{
				SetVisibility(ESlateVisibility::Visible);
				AddToViewport(98);
				FInputModeGameAndUI InputMode;
				InputMode.SetWidgetToFocus(TakeWidget());
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = true;
			}
			else
			{
				SetVisibility(ESlateVisibility::Hidden);
				FInputModeGameOnly InputMode;
				PC->SetInputMode(InputMode);
				PC->bShowMouseCursor = false;
			}
		}
	}
}

void UMainInventoryWidget::HandleSlotPressed(const FInventorySlotEntry& ItemEntry, int32 SlotIndex, FVector2D ScreenPosition)
{
	if(ItemActionWidget)
	{
		FVector2D PixelPosition;
		FVector2D ViewportPosition;
		USlateBlueprintLibrary::AbsoluteToViewport(this, ScreenPosition, PixelPosition, ViewportPosition);

		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemActionWidget->Slot))
		{
			CanvasSlot->SetPosition(ViewportPosition + FVector2D(20.f, 0.f));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("ItemActionWidget is not a CanvasPanelSlot!"));
		}

		//UE_LOG(LogTemp, Warning, TEXT("UMainInventoryWidget::HandleSlotPressed: Showing ItemActionWidget for Item: %s at Slot Index: %d"), *ItemEntry.GetItemDataName().ToString(), SlotIndex);
		ItemActionWidget->ShowWidget(ItemEntry, SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemActionWidget is not initialized!"));
	}
}

void UMainInventoryWidget::HandleDragStarted()
{
	if(ItemActionWidget)
	{
		//UE_LOG(LogTemp, Warning, TEXT("UMainInventoryWidget::HandleDragStarted: Hiding ItemActionWidget"));
		ItemActionWidget->HideWidget();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ItemActionWidget is not initialized!"));
	}
}

void UMainInventoryWidget::UpdateInventoryUI(const FInventorySlotEntry& ItemEntry, int32 SlotIndex, bool ItemSwaped)
{
	if(InventoryGrid)
	{
		//UE_LOG(LogTemp, Log, TEXT("Updating Inventory UI for Item: %s at Slot Index: %d"), *ItemEntry.GetItemDataName().ToString(), SlotIndex);
		InventoryGrid->AddItemDataToSlot(ItemEntry, SlotIndex, ItemSwaped);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryGrid is not initialized!"));
	}
}

void UMainInventoryWidget::UpdateInventoryUI_Remove(FName ItemName, int32 Quantity, int32 SlotIndex)
{
	if(InventoryGrid)
	{
		//UE_LOG(LogTemp, Log, TEXT("Updating Inventory UI for Item: %s at Slot Index: %d"), *ItemEntry.GetItemDataName().ToString(), SlotIndex);
		InventoryGrid->RemoveItemFromSlot(ItemName, Quantity, SlotIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryGrid is not initialized!"));
	}
}

void UMainInventoryWidget::SetInventoryGridMaxSlotCount(int32 NewSlotCount)
{
	UE_LOG(LogTemp, Error, TEXT("Setting Inventory Grid Max Slot Count to: %d"), NewSlotCount);
	if(InventoryGrid)
	{
		InventoryGrid->SetInventoryMaxSlotCount(NewSlotCount);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryGrid is not initialized!"));
	}
}
