// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/InventorySlotWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

UInventorySlotWidget::UInventorySlotWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ItemSlotData = FInventorySlotEntry();
	// Set Focusable to true to allow the widget to receive focus and handle input events
	SetIsFocusable(true);
}

void UInventorySlotWidget::NativePreConstruct()
{
	UpdateSlotUI();
	ItemSlotBorder->SetBrushColor(ButtonColor);
}

void UInventorySlotWidget::NativeConstruct()
{
}

bool UInventorySlotWidget::IsSlotValid() const
{	
	return ItemSlotData.IsValidEntry();
}

bool UInventorySlotWidget::CompareItemData(const FInventorySlotEntry& ItemData) const
{
	return ItemSlotData.ItemData == ItemData.ItemData;
}

void UInventorySlotWidget::AddItemDataToSlot(const FInventorySlotEntry& ItemData)
{
	if(!ItemData.IsValidEntry())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventorySlotWidget::AddItemDataToSlot: Attempted to add invalid item data to slot"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::AddItemDataToSlot: Adding item %s to slot"), *ItemData.GetItemDataName().ToString());
	ItemSlotData = ItemData;
	UpdateSlotUI();
}

void UInventorySlotWidget::AddQuantityToSlot(int32 QuantityToAdd)
{
	if(!ItemSlotData.IsValidEntry())
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventorySlotWidget::AddQuantityToSlot: Attempted to add quantity to an invalid slot"));
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::AddQuantityToSlot: Adding quantity %d to item %s in slot"), QuantityToAdd, *ItemSlotData.GetItemDataName().ToString());
	ItemSlotData.Quantity += QuantityToAdd;
	UpdateSlotUI();
}

void UInventorySlotWidget::ClearSlot()
{
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::ClearSlot: Clearing slot for item %s"), *ItemSlotData.GetItemDataName().ToString());
	ItemSlotData = FInventorySlotEntry();
	UpdateSlotUI();
}

void UInventorySlotWidget::UpdateSlotUI()
{
	if(!ItemSlotData.IsValidEntry())
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::UpdateSlotUI: Updating slot UI for item %s with quantity %d"), *ItemSlotData.GetItemDataName().ToString(), ItemSlotData.Quantity);
	if(ItemSlotData.ItemData->Icon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySlotWidget::UpdateSlotUI: Item %s has no icon!"), *ItemSlotData.GetItemDataName().ToString());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Stored ItemData: %s"),
		*GetNameSafe(ItemSlotData.ItemData));

	ItemIcon->SetBrushFromTexture(ItemSlotData.ItemData->Icon);
	ItemQuantityText->SetText(FText::AsNumber(ItemSlotData.Quantity));
}
