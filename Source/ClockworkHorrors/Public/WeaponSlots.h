// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponSlots.generated.h"

class UWeaponPickup;
class ACharacter;

UCLASS(ClassGroup = (Weapons), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UWeaponSlots : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponSlots();

	UFUNCTION(BlueprintCallable, Category = "Weapon Slots")
	bool AddWeapon(UWeaponPickup* NewWeaponPickup);

	UFUNCTION(BlueprintCallable, Category = "Weapon Slots")
	bool EquipSlot(int32 SlotNumber);

	UFUNCTION(BlueprintCallable, Category = "Weapon Slots")
	void DropEquippedWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon Slots")
	bool HasWeaponInSlot(int32 SlotNumber) const;

	UFUNCTION(BlueprintPure, Category = "Weapon Slots")
	AActor* GetWeaponInSlot(int32 SlotNumber) const;

	UFUNCTION(BlueprintPure, Category = "Weapon Slots")
	AActor* GetEquippedWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Weapon Slots")
	int32 GetEquippedSlot() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Slots", meta = (ClampMin = "1", ClampMax = "9"))
	int32 MaximumWeaponSlots = 9;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	TArray<UWeaponPickup*> StoredWeaponPickups;

	int32 EquippedSlotNumber;

	bool bSlotInputBindingsCreated;

	void SetupSlotInput();

	void HandleSlotOnePressed();
	void HandleSlotTwoPressed();
	void HandleSlotThreePressed();
	void HandleSlotFourPressed();
	void HandleSlotFivePressed();
	void HandleSlotSixPressed();
	void HandleSlotSevenPressed();
	void HandleSlotEightPressed();
	void HandleSlotNinePressed();

	void HandleDropPressed();

	UWeaponPickup* GetWeaponPickupInSlot(int32 SlotNumber) const;
};
