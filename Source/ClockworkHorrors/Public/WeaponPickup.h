// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WeaponPickup.generated.h"

class USphereComponent;
class UPrimitiveComponent;
class ACharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponEquippedStateChanged, bool, bWeaponEquipped);

UCLASS(ClassGroup = (Weapons), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UWeaponPickup : public UActorComponent
{
	GENERATED_BODY()

public:
	UWeaponPickup();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon | Pickup")
	class UInventoryItemDataAsset* ItemDataAsset;
	UFUNCTION(BlueprintCallable, Category = "Weapon|Pickup")
	bool PickupWeapon(ACharacter* NewHolder);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Pickup")
	void DropWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Inventory")
	bool StoreWeapon(ACharacter* NewHolder);

	UFUNCTION(BlueprintCallable, Category = "Weapon|Inventory")
	bool EquipStoredWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon|Inventory")
	void UnequipStoredWeapon();

	UFUNCTION(BlueprintPure, Category = "Weapon|Pickup")
	bool IsEquipped() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Inventory")
	bool IsStored() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Pickup")
	ACharacter* GetEquippedHolder() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Inventory")
	ACharacter* GetStoredHolder() const;

	UFUNCTION(BlueprintPure, Category = "Weapon|Inventory")
	int32 GetInventorySlot() const;

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Pickup")
	FWeaponEquippedStateChanged OnWeaponEquippedStateChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Inventory", meta = (ClampMin = "1", ClampMax = "9"))
	int32 InventorySlot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	FName GripSocketName = TEXT("weapon_r");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	FVector EquippedRelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	FRotator EquippedRelativeRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	FVector EquippedRelativeScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup", meta = (ClampMin = "1.0"))
	float PickupRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	float DropForwardDistance = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	float DropHeight = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Pickup")
	bool bBeginWithPhysics = true;
	void HandleInteractPressed();
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandlePickupVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void HandlePickupVolumeExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

private:
	UPROPERTY()
	USphereComponent* PickupVolume;

	UPROPERTY()
	UPrimitiveComponent* WeaponPhysicsComponent;

	UPROPERTY()
	ACharacter* NearbyPlayer;

	UPROPERTY()
	ACharacter* WeaponHolder;

	bool bWeaponEquipped;

	void CreatePickupVolume();
	void FindWeaponPhysicsComponent();

	

	void EnableWorldWeaponState();
	void DisableWorldWeaponState();

	bool GripSocketAlreadyOccupied(ACharacter* ProspectiveHolder) const;
};