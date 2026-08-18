// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utils/InventoryItemEntryStruct.h"
#include "InventorySlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventorySlotWidget(const FObjectInitializer& ObjectInitializer);

	void NativePreConstruct() override;

	void NativeConstruct() override;

	UFUNCTION()
	bool IsSlotValid() const;

	UFUNCTION()
	bool CompareItemData(const FInventorySlotEntry& ItemData) const;

	UFUNCTION()
	void AddItemDataToSlot(const FInventorySlotEntry& ItemData);

	UFUNCTION()
	void AddQuantityToSlot(int32 QuantityToAdd);

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	void ClearSlot();

	UFUNCTION(BlueprintCallable, Category = "InventorySlot")
	void UpdateSlotUI();

	UFUNCTION()
	void SetSlotIndex(int32 NewIndex) { SlotIndex = NewIndex; }

	UFUNCTION()
	FName GetItemDataName() const { return ItemSlotData.GetItemDataName(); }

	bool operator==(const UInventorySlotWidget& Other) const
	{
		return ItemSlotData.GetItemDataName() == Other.ItemSlotData.GetItemDataName();
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventorySlot")
	FInventorySlotEntry ItemSlotData;

	UPROPERTY(EditAnywhere, Category = "InventorySlot")
	FLinearColor ButtonColor;

	UPROPERTY(meta = (BindWidget))
	class UButton* ItemSlotButton;

	UPROPERTY(meta = (BindWidget))
	class UBorder* ItemSlotBorder;

	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ItemQuantityText;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* EquippedOverlay;

	UPROPERTY()
	int32 SlotIndex = -1;
	
};
