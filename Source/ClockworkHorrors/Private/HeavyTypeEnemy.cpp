// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "HeavyTypeEnemy.h"

#include "BaseCharacter.h"
#include "Engine/DamageEvents.h"


AHeavyTypeEnemy::AHeavyTypeEnemy()
{
	// =========================================================
	// DEFAULT STATS
	// =========================================================

	MoveSpeed = 150.0f;

	DetectionRadius = 750.0f;

	AttackRange = 175.0f;

	AttackDamage = 0.75f;

	AttackCooldown = 2.5f;


	// Heavy-specific defaults
	KnockbackForce = 900.0f;

	KnockbackUpForce = 250.0f;
}


// =========================================================
// PERFORM ATTACK
// =========================================================

void AHeavyTypeEnemy::PerformAttack()
{
	if (!TargetActor)
	{
		return;
	}


	ABaseCharacter* PlayerCharacter =
		Cast<ABaseCharacter>(TargetActor);


	if (!PlayerCharacter)
	{
		return;
	}


	bCanDealDamage = true;


	// =========================================================
	// DAMAGE
	// =========================================================

	PlayerCharacter->TakeDamage(
		AttackDamage,
		FDamageEvent(),
		GetController(),
		this
	);


	// =========================================================
	// KNOCKBACK
	// =========================================================

	FVector KnockbackDirection =
		PlayerCharacter->GetActorLocation() -
		GetActorLocation();


	KnockbackDirection.Z = 0.0f;


	if (!KnockbackDirection.IsNearlyZero())
	{
		KnockbackDirection.Normalize();


		FVector LaunchVelocity =
			KnockbackDirection *
			KnockbackForce;


		LaunchVelocity.Z =
			KnockbackUpForce;


		PlayerCharacter->LaunchCharacter(
			LaunchVelocity,
			true,
			true
		);
	}


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s performed HEAVY attack for %.2f damage."),
		*GetName(),
		AttackDamage
	);


	OnHeavyAttack();
}