// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "BaseBlaster.h"
#include "Components/BoxComponent.h"
#include "Projectile.h"
#include "WeaponPickup.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

// Sets default values
ABaseBlaster::ABaseBlaster()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	ActionHappening = false;

}

// Called when the game starts or when spawned
void ABaseBlaster::BeginPlay()
{
	Super::BeginPlay();
	//upcast
	AActor* Actor = this->GetParentActor();
	// downcast
	ParentPawn = Cast<APawn>(Actor);
	if (!ParentPawn)
	{

	}

	reloadAmmo();
}



void ABaseBlaster::Attack()
{
	ABaseWeapon::Attack();

	if (CanAttack())
	{
		FActorSpawnParameters Params;
		Params.Instigator = ParentPawn;
		Params.Owner = ParentPawn->GetController();
		AActor* spawnedActor = GetWorld()->SpawnActor<AActor>(projectileClass, skeletalMesh->GetSocketLocation(FName("MuzzleSocket")), ParentPawn->GetBaseAimRotation(), Params);
		projectile = Cast<AProjectile>(spawnedActor);
		FTimerHandle shootTimer;
		ActionHappening = true;
		GetWorld()->GetTimerManager().SetTimer(shootTimer, this, &ABaseBlaster::ActionStopped, 1.0f);
		OnDelegateInstance.Broadcast();
		useAmmo();
	}
}



const bool ABaseBlaster::CanAttack()
{
	return !ActionHappening && Alive && currentAmmo > 0;
}

void ABaseBlaster::useAmmo()
{
	currentAmmo = FMath::Max(currentAmmo - 1, 0);
	OnAmmoChange.Broadcast(currentAmmo, maxAmmo);
}



void ABaseBlaster::requestReload()
{
	if (!ActionHappening)
	{
		ActionHappening = true;
	}
	OnReloadStart.Broadcast();
}

void ABaseBlaster::reloadAmmo()
{
	currentAmmo = maxAmmo;
	OnAmmoChange.Broadcast(currentAmmo, maxAmmo);
}


float ABaseBlaster::getCurrAmmo()
{
	return currentAmmo;
}

float ABaseBlaster::getMaxAmmo()
{
	return maxAmmo;
}

void ABaseBlaster::setMaxAmmo(float newMax)
{
	maxAmmo = newMax;
}