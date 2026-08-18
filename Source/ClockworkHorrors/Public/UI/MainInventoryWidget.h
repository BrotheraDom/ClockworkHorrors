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

	UFUNCTION(BlueprintCallable)
	void ShowInventory(bool bInventoryShown);

	UFUNCTION()
	void UpdateInventoryUI(const FInventorySlotEntry& ItemEntry, int32 SlotIndex, bool ItemSwaped);

	UFUNCTION()
	void SetInventoryGridMaxSlotCount(int32 NewSlotCount);
	
};
