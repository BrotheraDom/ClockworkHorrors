// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWeaponDelegate);
UCLASS()
class CLOCKWORKHORRORS_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseWeapon();
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName WeaponSocket;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USkeletalMeshComponent* skeletalMesh;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UStaticMeshComponent* staticMesh;
	APawn* ParentPawn;
	/*UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UWeaponPickup* weaponPickup;*/
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool ActionHappening;
	virtual const bool CanAttack();
	FVector GetSource();
	void OwnerDied();
	UFUNCTION()
	void HandleWeaponEquippedStateChanged(bool bWeaponEquipped);
	bool Alive;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float damage;
public:	
	virtual void Attack();
	FWeaponDelegate  OnDelegateInstance;
	FWeaponDelegate OnActionDelegateInstance;
	UFUNCTION()
	void ActionStopped();
	float GetDamage();
private:
	
};
