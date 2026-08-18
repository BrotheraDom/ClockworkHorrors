// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/InventorySlotWidget.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Utils/InventoryDragDropOperation.h"
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
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::AddItemDataToSlot: ADDING NEW item %s to slot"), *ItemData.GetItemDataName().ToString());
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
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::AddQuantityToSlot: INCREASING quantity %d to item %s in slot"), QuantityToAdd, *ItemSlotData.GetItemDataName().ToString());
	ItemSlotData.Quantity += QuantityToAdd;
	UpdateSlotUI();
}

void UInventorySlotWidget::ClearSlot()
{
	//UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::ClearSlot: Clearing slot for item %s"), *ItemSlotData.GetItemDataName().ToString());
	ItemSlotData = FInventorySlotEntry();
	UpdateSlotUI();
}

void UInventorySlotWidget::UpdateSlotUI()
{
	if(!ItemSlotData.IsValidEntry())
	{
		return;
	}
	UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::UpdateSlotUI: UPDATING UI SLOT for item %s with quantity %d"), *ItemSlotData.GetItemDataName().ToString(), ItemSlotData.Quantity);
	if(ItemSlotData.ItemData->Icon == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySlotWidget::UpdateSlotUI: Item %s has no icon!"), *ItemSlotData.GetItemDataName().ToString());
		return;
	}

	/*UE_LOG(LogTemp, Warning, TEXT("Stored ItemData: %s"),
		*GetNameSafe(ItemSlotData.ItemData));*/

	ItemIcon->SetBrushFromTexture(ItemSlotData.ItemData->Icon);
	ItemQuantityText->SetText(FText::AsNumber(ItemSlotData.Quantity));
}

FReply UInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton) && ItemSlotData.IsValidEntry())
	{
		//Broadcast the OnPressEvent delegate with ItemSlotData as parameter and Slot Vector as parameter
		const FVector2D SlotScreenPosition = InGeometry.GetAbsolutePosition();
		OnPressEvent.Broadcast(ItemSlotData, SlotIndex, SlotScreenPosition);
		UE_LOG(LogTemp, Log, TEXT("UInventorySlotWidget::NativeOnMouseButtonDown: Right mouse button down on slot with item %s at index %d"), *ItemSlotData.GetItemDataName().ToString(), SlotIndex);
		return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::RightMouseButton).NativeReply;
	}

	//UE_LOG(Game, Warning, TEXT("UInventorySlot::NativeOnMouseButtonDown called"));
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	//UE_LOG(Game, Warning, TEXT("UInventorySlot::NativeOnDragDetected ATTEMPT! called for slot: %s"), *ItemName.ToString());
	
	UInventoryDragDropOperation* DragOperation = NewObject<UInventoryDragDropOperation>();
	if (DragOperation && ItemSlotData.IsValidEntry())
	{
		OnDragStarted.Broadcast();

		//UE_LOG(LogTemp, Warning, TEXT("item is equpped: %s"), (ItemData.bIsEquipped ? TEXT("true") : TEXT("false")));
		if (ItemSlotData.bIsEquipped)
		{
			//this->EnableEquippedOverlay(false);
		}
		//UE_LOG(Game, Warning, TEXT("Drag Detected: %s"), *ItemName.ToString());
		UInventorySlotWidget* VisualClone = CreateWidget<UInventorySlotWidget>(GetWorld(), GetClass());
		VisualClone->AddItemDataToSlot(ItemSlotData);
		DragOperation->DefaultDragVisual = VisualClone;
		DragOperation->SourceInventorySlot = this;
		OutOperation = DragOperation;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UInventorySlot::NativeOnDragDetected FAILED!!! to create drag operation or slot is not occupied!"));
		//OutOperation = nullptr;
	}
	
}

bool UInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	
	if (UInventoryDragDropOperation* DragOp = Cast<UInventoryDragDropOperation>(InOperation))
	{
		//UE_LOG(Game, Warning, TEXT("Drop Detected: %s"), *ItemName.ToString());
		if (DragOp->SourceInventorySlot && DragOp->SourceInventorySlot != this)
		{
			//UE_LOG(Game, Warning, TEXT("Swapping items between slots: %s and %s"), *DragOp->SourceInventorySlot->ItemName.ToString(), *ItemName.ToString());

			// Swap logic
			UInventorySlotWidget* FromSlot = DragOp->SourceInventorySlot;

			OnSwapEvent.Broadcast(FromSlot->SlotIndex, SlotIndex);

			return true;
		}
	}
	
	return false;
}
