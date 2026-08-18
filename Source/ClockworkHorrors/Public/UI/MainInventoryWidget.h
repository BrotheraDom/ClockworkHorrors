// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utils/InventoryItemEntryStruct.h"
#include "MainInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UMainInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMainInventoryWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	class UInventoryGridWidget* InventoryGrid;

	UPROPERTY(meta = (BindWidget))
	class UItemActionWidget* ItemActionWidget;

	UFUNCTION(BlueprintCallable)
	void ShowInventory(bool bInventoryShown);

	UFUNCTION(BlueprintCallable)
	void HandleSlotPressed(const FInventorySlotEntry& ItemEntry, int32 SlotIndex, FVector2D ScreenPosition);

	UFUNCTION(BlueprintCallable)
	void HandleDragStarted();

	UFUNCTION()
	void UpdateInventoryUI(const FInventorySlotEntry& ItemEntry, int32 SlotIndex, bool ItemSwaped);

	UFUNCTION()
	void UpdateInventoryUI_Remove(FName ItemName, int32 Quantity, int32 SlotIndex);

	UFUNCTION()
	void SetInventoryGridMaxSlotCount(int32 NewSlotCount);
	
};
