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

	bInflictsImpactDamage = true;
	ImpactDamageStrength = 1.0f;

	bRestoresImpactHealth = false;
	ImpactHealthRestoration = 1.0f;

	bCreateLingeringField = false;
	LingeringFieldLifetimeSeconds = 5.0f;
	LingeringFieldRadius = 250.0f;
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