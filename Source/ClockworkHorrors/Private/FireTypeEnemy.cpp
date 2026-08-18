// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "FireTypeEnemy.h"

#include "BaseCharacter.h"
#include "Engine/DamageEvents.h"


AFireTypeEnemy::AFireTypeEnemy()
{
	// ---------------------------------------------------------
	// FIRE ENEMY DEFAULT STATS
	// ---------------------------------------------------------

	MoveSpeed = 275.0f;

	DetectionRadius = 900.0f;

	AttackRange = 400.0f;

	AttackDamage = 0.30f;

	AttackCooldown = 1.0f;

	BurnDamage = 0.05f;

	BurnDuration = 3.0f;
}


// =========================================================
// FIRE ATTACK
// =========================================================

void AFireTypeEnemy::PerformAttack()
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
		TEXT("%s performed FIRE attack for %.2f damage."),
		*GetName(),
		AttackDamage
	);


	// Blueprint handles visuals / audio.
	OnFireAttack();
}