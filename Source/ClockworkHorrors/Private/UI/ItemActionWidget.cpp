// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/ItemActionWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ButtonWithText.h"
#include "BaseCharacter.h"
#include "Utils/InventoryComponent.h"

UItemActionWidget::UItemActionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsFocusable(true);
}

void UItemActionWidget::NativePreConstruct()
{
}

void UItemActionWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentSlotIndex = -1;

	if (EquipSwitcher)
	{
		EquipSwitcher->SetActiveWidgetIndex(0);
	}

	if(EquipButton)
	{
		EquipButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnEquipActionButtonClicked);
	}
	if (UnequipButton)
	{
		UnequipButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnUnequipActionButtonClicked);
	}

	if(UseButton)
	{
		UseButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnUseActionButtonClicked);
	}

	if (DropButton)
	{
		DropButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnDropActionButtonClicked);
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemActionWidget::NativeConstruct: World is null!"));
		return;
	}
	APlayerController* const PC = World->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemActionWidget::NativeConstruct: PlayerController is null!"));
		return;
	}
	ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(PC->GetPawn());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemActionWidget::NativeConstruct: PlayerCharacter is null!"));
		return;
	}

	UInventoryComponent* InventoryComponent = PlayerCharacter->GetComponentByClass<UInventoryComponent>();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("UItemActionWidget::NativeConstruct: InventoryComponent is null!"));
		return;
	}
	OnItemActionClicked.AddDynamic(InventoryComponent, &UInventoryComponent::HandleItemAction);
}

void UItemActionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UItemActionWidget::ShowWidget(const FInventorySlotEntry& ItemEntry, int32 SlotIndex)
{
	if(!ItemEntry.IsValidEntry())
	{
		UE_LOG(LogTemp, Warning, TEXT("UItemActionWidget::ShowWidget: Attempted to show widget for invalid item entry"));
		return;
	}
	CurrentSlotIndex = SlotIndex;
	SetVisibility(ESlateVisibility::Visible);

	// Determine which actions to show based on the item entry
	if (ItemEntry.ItemData->bIsEquippable && ItemEntry.bIsEquipped) // If the item is equippable and currently equipped Show the Unequip button
	{
		EquipSwitcher->SetActiveWidgetIndex(1); // Switch to Unequip button
		ShowEquipSwitcherAction();
	}
	else if (ItemEntry.ItemData->bIsEquippable && !ItemEntry.bIsEquipped) // If the item is equippable and not currently equipped Show the Equip button
	{
		EquipSwitcher->SetActiveWidgetIndex(0); // Switch to Equip button
		ShowEquipSwitcherAction();
	}
	else if (!ItemEntry.ItemData->bIsEquippable) // If the item is not equippable, hide the EquipSwitcher
	{
		HideEquipSwitcherAction();
	}

	if(ItemEntry.ItemData->bIsHealthItem || ItemEntry.ItemData->bIsThrowable || ItemEntry.ItemData->bIsAttachment) // If the item is usable, show the Use button
	{
		ShowUseAction();
	}
	else
	{
		HideUseAction();
	}
	ShowDropAction(); // Always show the Drop button FOR NOW, but this can be changed later if needed
}

void UItemActionWidget::ShowEquipSwitcherAction()
{
	if (!EquipSwitcher)
	{
		return;
	}

	EquipSwitcher->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::ShowUseAction()
{
	if(!UseButton)
	{
		return;
	}

	UseButton->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::ShowDropAction()
{
	if(!DropButton)
	{
		return;
	}

	DropButton->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
	CurrentSlotIndex = -1;
}

void UItemActionWidget::HideEquipSwitcherAction()
{
	if(!EquipSwitcher)
	{
		return;
	}

	EquipSwitcher->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::HideUseAction()
{
	if(!UseButton)
	{
		return;
	}

	UseButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::HideDropAction()
{
	if(!DropButton)
	{
		return;
	}

	DropButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::ResetMenu()
{
	EquipSwitcher->SetActiveWidgetIndex(0);
	ShowEquipSwitcherAction();
	ShowUseAction();
	ShowDropAction();
	CurrentSlotIndex = -1;
}

void UItemActionWidget::OnEquipActionButtonClicked()
{
	if(!EquipSwitcher && !EquipButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(0, CurrentSlotIndex);
	EquipSwitcher->SetActiveWidgetIndex(1); // Switch to Unequip button

	HideWidget();
}

void UItemActionWidget::OnUnequipActionButtonClicked()
{
	if(!EquipSwitcher && !UnequipButton)
	{
		return;
	}
	
	OnItemActionClicked.Broadcast(1, CurrentSlotIndex);
	EquipSwitcher->SetActiveWidgetIndex(0); // Switch to Equip button

	HideWidget();
}

void UItemActionWidget::OnUseActionButtonClicked()
{
	if(!UseButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(2, CurrentSlotIndex);

	HideWidget();
}

void UItemActionWidget::OnDropActionButtonClicked()
{
	if(!DropButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(3, CurrentSlotIndex);

	HideWidget();
}
