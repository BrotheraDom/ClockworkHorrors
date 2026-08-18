// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "IceTypeEnemy.h"

#include "BaseCharacter.h"
#include "Engine/DamageEvents.h"


AIceTypeEnemy::AIceTypeEnemy()
{
	// =========================================================
	// DEFAULT STATS
	// =========================================================

	MoveSpeed = 225.0f;

	DetectionRadius = 850.0f;

	AttackRange = 450.0f;

	AttackDamage = 0.20f;

	AttackCooldown = 1.25f;


	// Ice-specific defaults
	SlowMultiplier = 0.50f;

	SlowDuration = 2.5f;
}


// =========================================================
// PERFORM ATTACK
// =========================================================

void AIceTypeEnemy::PerformAttack()
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


	PlayerCharacter->TakeDamage(
		AttackDamage,
		FDamageEvent(),
		GetController(),
		this
	);


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s performed ICE attack for %.2f damage."),
		*GetName(),
		AttackDamage
	);


	OnIceAttack();
}