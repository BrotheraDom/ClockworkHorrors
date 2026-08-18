// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemDataAsset.h"
#include "InventoryItemEntryStruct.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FInventorySlotEntry
{
	GENERATED_BODY()

	FInventorySlotEntry();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryItemDataAsset> ItemData = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	bool bIsEquipped = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 CurrentBind = -1;

	UPROPERTY()
	bool bIsMaxStacked = false;

	bool IsValidEntry() const
	{
		return ItemData != nullptr && ItemData->IsValidItem();
	}

	bool operator==(const FInventorySlotEntry& Other) const
	{
		return ItemData && Other.ItemData && ItemData->GetItemName() == Other.ItemData->GetItemName();
	}

	FName GetItemDataName() const
	{
		return ItemData ? ItemData->GetItemName() : NAME_None;
	}

	bool GetCanStack() const
	{
		return ItemData ? ItemData->bIsStackable : false;
	}
};

//class CLOCKWORKHORRORS_API InventoryItemEntryStruct
//{
//public:
//	InventoryItemEntryStruct();
//	~InventoryItemEntryStruct();
//};
