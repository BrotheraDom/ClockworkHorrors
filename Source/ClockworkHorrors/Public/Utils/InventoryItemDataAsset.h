// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryItemDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CLOCKWORKHORRORS_API UInventoryItemDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UInventoryItemDataAsset();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FName ItemName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	UTexture2D* Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags")
	bool bIsEquippable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags")
	bool bIsThrowable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags", meta=(EditCondition = "!bIsEquippable"))
	bool bIsHealthItem = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags", meta = (EditCondition = "!bIsEquippable"))
	bool bIsAttachment = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags")
	bool bIsCombinable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags", meta = (EditCondition = "!bIsEquippable"))
	bool bIsStackable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Flags")
	bool bIsCraftingMaterial = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Classes", meta = (EditCondition = "bIsEquippable"))
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Classes", meta = (EditCondition = "bIsEquippable"))
	TSubclassOf<AActor> ThrowClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Values", meta = (EditCondition = "bIsHealthItem"))
	float HealthRestoreAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Values", meta = (EditCondition = "bIsEquippable"))
	float DamageAmount = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Values", meta = (EditCondition = "!bIsEquippable"))
	int32 Quantity = 1;

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Values")
	float DurationAmount = 0.0f;*/

	bool IsValidItem() const
	{
		return ItemName != NAME_None && Icon != nullptr;
	}

	FName GetItemName() const
	{
		return ItemName;
	}

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("InventoryItem"), GetFName());
	}
};
//UCLASS()
//class CLOCKWORKHORRORS_API UInventoryItemDataAsset : public UPrimaryDataAsset
//{
//	GENERATED_BODY()
//
//public:
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	FName ItemName;
//
//	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	int32 SlotSpaces;*/
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	UTexture2D* Icon;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	TSubclassOf<AActor> BlueprintClass;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	TSubclassOf<AActor> ThrowClass;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	bool bIsEquippable = false;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	bool bIsStackable = false;
//
//	/*UPROPERTY(BlueprintReadOnly, Category = "Inventory")
//	int32 CurrentBind = -1;*/
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	bool bIsThrowable = false;
//
//	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
//	bool bIsAttachment = false;
//
//	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	FAttachmentData AttachmentData;*/
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	bool bIsHealthItem = false;
//
//	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
//	float HealthRestoreAmount = 0.0f;
//
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	bool bIsCombinable = false;
//
//	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
//	float DurationAmount = 0.0f;
//
//
//
//	/*
//	
//	MOVE THIS TO INVENTORY SLOT STRUCT
//	
//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
//	int32 Quantity;
//
//	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
//	bool bOccupied = false;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Inventory")
//	bool bIsEquipped = false;
//
//	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
//	bool bIsOriginalSlot = false;
//
//	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
//	bool bIsPartOfMultipleSlots = false;
//
//	UPROPERTY(BlueprintReadWrite, Category = "Inventory")
//	TArray<int32> ConnectedSlotIndexes;
//	*/
//	
//	UInventoryItemDataAsset();
//
//	/*UFUNCTION(BlueprintCallable, Category = "Inventory")
//	void SetOccupied(bool bOccupied);*/
//
//};
