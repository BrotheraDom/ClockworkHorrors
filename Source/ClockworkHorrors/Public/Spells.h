// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spells.generated.h"

class ASpellProjectile;

UENUM(BlueprintType)
enum class ESpellCastForm : uint8
{
	Self UMETA(DisplayName = "Self"),
	Projectile UMETA(DisplayName = "Projectile")
};

UENUM(BlueprintType)
enum class ELingeringFieldShape : uint8
{
	Sphere UMETA(DisplayName = "Sphere"),
	Square UMETA(DisplayName = "Square")
};

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API ASpells : public AActor
{
	GENERATED_BODY()

public:
	ASpells();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Identity")
	FName SpellName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Casting")
	ESpellCastForm CastForm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Casting",
		meta = (ClampMin = "0.0"))
	float CastCooldownSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0"))
	float MaximumTravelDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0"))
	float ProjectileTravelSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "1.0"))
	float ProjectileCollisionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.1"))
	float ProjectileExistenceSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0"))
	float ProjectileGravityStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	TSubclassOf<ASpellProjectile> ProjectileActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Behavior", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bProjectilePierces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Pierce", meta = (EditCondition = "bProjectilePierces", EditConditionHides))
	bool bLimitProjectilePierces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Pierce", meta = (EditCondition = "bProjectilePierces && bLimitProjectilePierces", EditConditionHides, ClampMin = "1"))
	int32 MaximumProjectilePierces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Pierce", meta = (EditCondition = "bProjectilePierces", EditConditionHides))
	bool bModifyDamagePerPierce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Pierce", meta = (EditCondition = "bProjectilePierces && bModifyDamagePerPierce", EditConditionHides, ClampMin = "0.0"))
	float DamageMultiplierPerPierce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Behavior", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bProjectileBounces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Behavior", meta = (EditCondition = "bProjectileBounces", EditConditionHides, ClampMin = "1"))
	int32 MaximumProjectileBounces;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Behavior", meta = (EditCondition = "bProjectileBounces", EditConditionHides, ClampMin = "0.0", ClampMax = "1.0"))
	float ProjectileBounciness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Homing", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bProjectileHoming;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Homing", meta = (EditCondition = "bProjectileHoming", EditConditionHides, ClampMin = "0.0"))
	float ProjectileHomingStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Homing", meta = (EditCondition = "bProjectileHoming", EditConditionHides, ClampMin = "1.0"))
	float ProjectileHomingDetectionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Multishot", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bProjectileMultishot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Multishot", meta = (EditCondition = "bProjectileMultishot", EditConditionHides, ClampMin = "2"))
	int32 ProjectileCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile|Multishot", meta = (EditCondition = "bProjectileMultishot", EditConditionHides, ClampMin = "0.0", ClampMax = "360.0"))
	float ProjectileSpreadAngle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bChargeableSpell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging", meta = (EditCondition = "bChargeableSpell", EditConditionHides, ClampMin = "0.0"))
	float MinimumChargeTimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging", meta = (EditCondition = "bChargeableSpell", EditConditionHides, ClampMin = "0.0"))
	float MaximumChargeTimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Damage", meta = (EditCondition = "bChargeableSpell", EditConditionHides))
	bool bChargeAffectsImpactDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Damage", meta = (EditCondition = "bChargeableSpell && bChargeAffectsImpactDamage", EditConditionHides, ClampMin = "0.0"))
	float MinimumChargeDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Damage", meta = (EditCondition = "bChargeableSpell && bChargeAffectsImpactDamage", EditConditionHides, ClampMin = "0.0"))
	float MaximumChargeDamageMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Size", meta = (EditCondition = "bChargeableSpell", EditConditionHides))
	bool bChargeAffectsProjectileSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Size", meta = (EditCondition = "bChargeableSpell && bChargeAffectsProjectileSize", EditConditionHides, ClampMin = "0.01"))
	float MinimumChargeSizeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Size", meta = (EditCondition = "bChargeableSpell && bChargeAffectsProjectileSize", EditConditionHides, ClampMin = "0.01"))
	float MaximumChargeSizeMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Speed", meta = (EditCondition = "bChargeableSpell", EditConditionHides))
	bool bChargeAffectsProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Speed", meta = (EditCondition = "bChargeableSpell && bChargeAffectsProjectileSpeed", EditConditionHides, ClampMin = "0.0"))
	float MinimumChargeSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Charging|Speed", meta = (EditCondition = "bChargeableSpell && bChargeAffectsProjectileSpeed", EditConditionHides, ClampMin = "0.0"))
	float MaximumChargeSpeedMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact")
	bool bInflictsImpactDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact", meta = (EditCondition = "bInflictsImpactDamage", EditConditionHides, ClampMin = "0.0"))
	float ImpactDamageStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact")
	bool bRestoresImpactHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact", meta = (EditCondition = "bRestoresImpactHealth", EditConditionHides, ClampMin = "0.0"))
	float ImpactHealthRestoration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides))
	bool bCreateLingeringField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides))
	ELingeringFieldShape LingeringFieldShape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides, ClampMin = "0.1"))
	float LingeringFieldLifetimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField && LingeringFieldShape == ELingeringFieldShape::Sphere", EditConditionHides, ClampMin = "1.0"))
	float LingeringFieldRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField && LingeringFieldShape == ELingeringFieldShape::Square", EditConditionHides, ClampMin = "1.0"))
	float LingeringFieldSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField && LingeringFieldShape == ELingeringFieldShape::Square", EditConditionHides, ClampMin = "1.0"))
	float LingeringFieldHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides, ClampMin = "0.05"))
	float LingeringPulseIntervalSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides))
	bool bLingeringFieldInflictsDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bLingeringFieldInflictsDamage", EditConditionHides, ClampMin = "0.0"))
	float LingeringDamagePerPulse;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides))
	bool bLingeringFieldRestoresHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bLingeringFieldRestoresHealth", EditConditionHides, ClampMin = "0.0"))
	float LingeringHealthPerPulse;

	UFUNCTION(BlueprintPure, Category = "Spell")
	bool CastsOnCaster() const;

	UFUNCTION(BlueprintPure, Category = "Spell")
	bool LaunchesProjectile() const;

	UFUNCTION(BlueprintPure, Category = "Spell")
	bool ProducesLingeringField() const;
};