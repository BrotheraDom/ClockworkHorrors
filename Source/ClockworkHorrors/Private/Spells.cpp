// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Spells.h"

ASpells::ASpells()
{
	PrimaryActorTick.bCanEverTick = false;

	SpellName = TEXT("UnnamedSpell");

	CastForm = ESpellCastForm::Projectile;
	CastCooldownSeconds = 1.0f;

	MaximumTravelDistance = 3000.0f;
	ProjectileTravelSpeed = 1500.0f;
	ProjectileCollisionRadius = 20.0f;
	ProjectileExistenceSeconds = 5.0f;
	ProjectileGravityStrength = 0.0f;
	ProjectileActorClass = nullptr;

	bProjectilePierces = false;
	bLimitProjectilePierces = false;
	MaximumProjectilePierces = 3;
	bModifyDamagePerPierce = false;
	DamageMultiplierPerPierce = 1.0f;

	bProjectileBounces = false;
	MaximumProjectileBounces = 3;
	ProjectileBounciness = 1.0f;

	bProjectileHoming = false;
	ProjectileHomingStrength = 5000.0f;
	ProjectileHomingDetectionRadius = 2000.0f;

	bProjectileMultishot = false;
	ProjectileCount = 3;
	ProjectileSpreadAngle = 20.0f;

	bChargeableSpell = false;
	MinimumChargeTimeSeconds = 0.0f;
	MaximumChargeTimeSeconds = 2.0f;

	bChargeAffectsImpactDamage = false;
	MinimumChargeDamageMultiplier = 1.0f;
	MaximumChargeDamageMultiplier = 2.0f;

	bChargeAffectsProjectileSize = false;
	MinimumChargeSizeMultiplier = 1.0f;
	MaximumChargeSizeMultiplier = 2.0f;

	bChargeAffectsProjectileSpeed = false;
	MinimumChargeSpeedMultiplier = 1.0f;
	MaximumChargeSpeedMultiplier = 1.5f;

	bInflictsImpactDamage = true;
	ImpactDamageStrength = 1.0f;

	bRestoresImpactHealth = false;
	ImpactHealthRestoration = 1.0f;

	bCreateLingeringField = false;
	LingeringFieldShape = ELingeringFieldShape::Sphere;
	LingeringFieldLifetimeSeconds = 5.0f;
	LingeringFieldRadius = 250.0f;
	LingeringFieldSize = 500.0f;
	LingeringFieldHeight = 100.0f;
	LingeringPulseIntervalSeconds = 1.0f;

	bLingeringFieldInflictsDamage = false;
	LingeringDamagePerPulse = 1.0f;

	bLingeringFieldRestoresHealth = false;
	LingeringHealthPerPulse = 1.0f;
}

bool ASpells::CastsOnCaster() const
{
	return CastForm == ESpellCastForm::Self;
}

bool ASpells::LaunchesProjectile() const
{
	return CastForm == ESpellCastForm::Projectile;
}

bool ASpells::ProducesLingeringField() const
{
	return bCreateLingeringField && (bLingeringFieldInflictsDamage || bLingeringFieldRestoresHealth);
}