// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utils/InventoryItemEntryStruct.h"
#include "InventoryGridWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventoryGridWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "InventoryGrid")
	void AddItemDataToSlot(const FInventorySlotEntry& ItemData, int32 SlotIndex, bool ItemSwaped);

	UFUNCTION(BlueprintCallable, Category = "InventoryGrid")
	void RemoveItemFromSlot(FName ItemName, int32 Quantity, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "InventoryGrid")
	void SetInventoryMaxSlotCount(int32 NewSlotCount);

	UFUNCTION(BlueprintCallable, Category = "InventoryGrid")
	void SetInventoryMainReference(class UMainInventoryWidget* NewMainInventoryReference) { MainInventoryReference = NewMainInventoryReference; }

protected:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	UFUNCTION()
	void EnsureSlotCount(int32 DesiredCount);

	UFUNCTION()
	class UHorizontalBox* GetOrCreateRowForIndex(int32 SlotIndex);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryGrid")
	TArray<class UInventorySlotWidget*> InventorySlotWidgets;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "InventoryGrid")
	TSubclassOf<class UInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UWrapBox* InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	class UVerticalBox* InventoryVerticalBox;

	UPROPERTY()
	TArray<class UHorizontalBox*> InventoryRows;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryGrid",
		meta = (ClampMin = "1", ToolTip = "Starting number of slots in the inventory grid."))
	int32 StartingSlotCount = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryGrid",
		meta = (ClampMin = "1", ToolTip = "Number of slots per row before wrapping to a new row."))
	int32 MaxRow = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryGrid",
		meta = (ClampMin = "1", ToolTip = "Number of rows. Ignored if bIsGrowable is true and no cap is desired."))
	int32 MaxColumn = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "InventoryGrid",
		meta = (ToolTip = "If false, the inventory grid can grow beyond the specified MaxRow and MaxColumn."))
	bool bIsFixed = false;

private:
	TMap<int32, class UInventorySlotWidget*> ItemNameToSlotWidgetMap;

	class UMainInventoryWidget* MainInventoryReference;

	bool IsSlotIndexValid(int32 SlotIndex) const;

	bool CanIncreaseSlotCount(int32 NewSlotCount) const;

	int32 MaxSlotCount = 99;
	
};
