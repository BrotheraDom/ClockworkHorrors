// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryItemEntryStruct.h"
#include "Interfaces/InteractInterface.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponPickedUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemDataAdded, const FInventorySlotEntry&, ItemEntry, int32, SlotIndex, bool, ItemSwaped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnItemRemoved, FName, ItemsName, int32, Quantities, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInventorySizeIncreased, int32, NewSize);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLOCKWORKHORRORS_API UInventoryComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta=(ToolTip="This map is used to define the maximum stack size for each item name in the inventory"))
	TMap<FName, int32> StackMax;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory", meta = (ToolTip = "This array is used to hold all items in the inventory"))
	TArray<FInventorySlotEntry> InventoryItems;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory", meta = (ToolTip = "This variable is used to define the maximum number of slots in the inventory"))
	int32 MaxSlots;

private:
	int32 CurrentAvailableSlots; // This variable is used to keep track of the current available slots in the inventory

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UInventoryItemDataAsset* Item);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemsByAmount(FName ItemName, int32 Quantity);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemByName(FName ItemName);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool HasItem(FName ItemName) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventorySlotEntry GetItem(FName ItemName) const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ShowInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClearInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 GetFirstAvailableSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int32 FindItemByName(FName ItemName) const;

	// DELEGATES
	UPROPERTY(BlueprintReadOnly, Category = "Variables|Delegate")
	FOnWeaponPickedUp OnWeaponPickedUp;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Variables|Delegate")
	FOnItemDataAdded OnItemDataAdded;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Variables|Delegate")
	FOnItemRemoved OnItemRemoved;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Variables|Delegate")
	FOnInventorySizeIncreased OnInventorySizeIncreased;

	/// Implementing the IInteractInterface
	void OnInteract() override;

		
};
