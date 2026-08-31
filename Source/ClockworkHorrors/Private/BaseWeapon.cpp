// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "BaseWeapon.h"

#include "BaseCharacter.h"
#include "Components/CharacterAnimationComponent.h"
#include "GameFramework/Character.h"
#include "WeaponPickup.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	skeletalMesh =
		CreateDefaultSubobject<USkeletalMeshComponent>("skeletalMesh");

	SetRootComponent(skeletalMesh);
	skeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ActionHappening = false;
	Alive = true;

	AnimationStance = ECharacterAnimationStance::Unarmed;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	UWeaponPickup* WeaponPickup = FindComponentByClass<UWeaponPickup>();

	if (IsValid(WeaponPickup))
	{
		WeaponPickup->OnWeaponEquippedStateChanged.AddDynamic(
			this,
			&ABaseWeapon::HandleWeaponEquippedStateChanged
		);
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
	UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>();

	// Cache the holder while the pickup still knows who owns it.
	if (bWeaponEquipped && IsValid(Pickup))
	{
		if (ACharacter* EquippedHolder = Pickup->GetEquippedHolder())
		{
			ParentPawn = EquippedHolder;
		}
	}

	if (!ParentPawn)
	{
		return;
	}

	// Animation stance is universal: any pawn with the reusable
	// animation component can respond to the equipped weapon.
	if (
		UCharacterAnimationComponent* AnimationComponent =
		ParentPawn->FindComponentByClass<UCharacterAnimationComponent>()
		)
	{
		if (bWeaponEquipped)
		{
			AnimationComponent->SetAnimationStance(AnimationStance);
		}
		else
		{
			AnimationComponent->ResetAnimationStance();
		}
	}

	// Preserve the project's existing BaseCharacter weapon pointer flow.
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(ParentPawn))
	{
		if (bWeaponEquipped)
		{
			Character->weapon = this;
		}
		else if (Character->weapon == this)
		{
			Character->weapon = nullptr;
		}
	}
}
