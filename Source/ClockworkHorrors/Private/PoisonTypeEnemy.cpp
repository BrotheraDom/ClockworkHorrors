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

UStatusEffectType* APoisonTypeEnemy::GetStatusEffectPayload() const
{

	UStatusEffectType* StatusEffect = NewObject<UStatusEffectType>();

	StatusEffect->Effect = STATUSEFFECT::Poisoned;
	StatusEffect->Duration = PoisonDuration;
	StatusEffect->TickDamage = PoisonTickDamage;
	StatusEffect->TickInterval = PoisonTickInterval;
	StatusEffect->Percentage = 0.0f;

	return StatusEffect;
}
