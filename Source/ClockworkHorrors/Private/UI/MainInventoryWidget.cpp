// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/MainInventoryWidget.h"
#include "BaseCharacter.h"
#include "UI/InventoryGridWidget.h"

UMainInventoryWidget::UMainInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMainInventoryWidget::NativeConstruct()
{
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
				if (!InventoryGrid)
				{
					UE_LOG(LogTemp, Error, TEXT("InventoryGrid is not initialized!"));
					return;
				}
				//InventoryGrid->HideOtherSlotsContext(-1);
			}
		}
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
