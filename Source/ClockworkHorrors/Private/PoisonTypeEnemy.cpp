// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "PoisonTypeEnemy.h"

#include "BaseCharacter.h"
#include "Engine/DamageEvents.h"


APoisonTypeEnemy::APoisonTypeEnemy()
{
	// =========================================================
	// DEFAULT STATS
	// =========================================================

	MoveSpeed = 250.0f;

	DetectionRadius = 800.0f;

	AttackRange = 350.0f;

	// Lower initial damage because this enemy
	// will eventually deal poison damage over time.
	AttackDamage = 0.10f;

	AttackCooldown = 1.5f;


	// Poison-specific defaults
	PoisonTickDamage = 0.05f;

	PoisonTickInterval = 1.0f;

	PoisonDuration = 5.0f;
}


// =========================================================
// PERFORM ATTACK
// =========================================================

void APoisonTypeEnemy::PerformAttack()
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
		TEXT(
			"%s performed POISON attack for %.2f initial damage."
		),
		*GetName(),
		AttackDamage
	);


	OnPoisonAttack();
}