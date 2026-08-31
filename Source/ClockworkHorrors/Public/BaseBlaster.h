// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseBlaster.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAmmoDelegate, float, current, float, max);
UCLASS()
class CLOCKWORKHORRORS_API ABaseBlaster : public ABaseWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseBlaster();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;




	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AActor> projectileClass;



	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int maxAmmo;
	void useAmmo();
	class AProjectile* projectile;
	virtual const bool CanAttack() override;
public:
	virtual void Attack() override;
	UPROPERTY(BlueprintAssignable, Category = "Blaster")
	FAmmoDelegate OnAmmoChange;
	UPROPERTY(BlueprintAssignable, Category = "Blaster")
	FWeaponDelegate OnReloadStart;
	UFUNCTION()
	void requestReload();
	UFUNCTION()
	void reloadAmmo();
	int getCurrAmmo();
	int getMaxAmmo();
	void setMaxAmmo(int newMax);
	void setCurrAmmo(int curAmmo);
private:
	int currentAmmo;

};