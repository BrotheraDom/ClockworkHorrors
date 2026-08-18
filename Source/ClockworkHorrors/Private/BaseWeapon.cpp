// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "BaseWeapon.h"
#include <WeaponPickup.h>
#include "GameFramework/Character.h"
#include <BaseCharacter.h>

// Sets default values
ABaseWeapon::ABaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	skeletalMesh = CreateDefaultSubobject< USkeletalMeshComponent>("skeletalMesh");
	SetRootComponent(skeletalMesh);
	skeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ActionHappening = false;
	Alive = true;
	//weaponPickup = CreateDefaultSubobject<UWeaponPickup>("weaponPickup");
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	UWeaponPickup* WeaponPickup = FindComponentByClass<UWeaponPickup>();

	if (IsValid(WeaponPickup))
	{
		WeaponPickup->OnWeaponEquippedStateChanged.AddDynamic(this, &ABaseWeapon::HandleWeaponEquippedStateChanged);
	}
}

const bool ABaseWeapon::CanAttack()
{
	return !ActionHappening && Alive;
}

void ABaseWeapon::Attack()
{
	if (!ParentPawn)
	{
		UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>();

		if (IsValid(Pickup) && Pickup->IsEquipped())
		{
			ParentPawn = Pickup->GetEquippedHolder();
		}
	}

	if (!ParentPawn)
	{
		return;
	}
}

void ABaseWeapon::ActionStopped()
{
	ActionHappening = false;
	OnActionDelegateInstance.Broadcast();
}

float ABaseWeapon::GetDamage()
{
	return damage;
}

FVector ABaseWeapon::GetSource()
{
	return skeletalMesh->GetSocketLocation(WeaponSocket);
}

void ABaseWeapon::OwnerDied()
{
	Alive = false;
}

void ABaseWeapon::HandleWeaponEquippedStateChanged(bool bWeaponEquipped)
{
	if (!ParentPawn)
	{
		UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>();

		if (IsValid(Pickup) && Pickup->IsEquipped())
		{
			ParentPawn = Pickup->GetEquippedHolder();
		}
	}
	ABaseCharacter* player = Cast<ABaseCharacter>(ParentPawn);
	player->weapon = this;
}
