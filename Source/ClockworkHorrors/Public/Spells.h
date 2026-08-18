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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0")) float MaximumTravelDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0")) float ProjectileTravelSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "1.0")) float ProjectileCollisionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.1")) float ProjectileExistenceSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides, ClampMin = "0.0")) float ProjectileGravityStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Projectile", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides)) TSubclassOf<ASpellProjectile> ProjectileActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact") 
	bool bInflictsImpactDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact", meta = (EditCondition = "bInflictsImpactDamage", EditConditionHides, ClampMin = "0.0"))
	float ImpactDamageStrength;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact") bool bRestoresImpactHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Impact", meta = (EditCondition = "bRestoresImpactHealth", EditConditionHides, ClampMin = "0.0")) 
	float ImpactHealthRestoration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "CastForm == ESpellCastForm::Projectile", EditConditionHides)) 
	bool bCreateLingeringField;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides, ClampMin = "0.1")) 
	float LingeringFieldLifetimeSeconds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spell|Lingering", meta = (EditCondition = "bCreateLingeringField", EditConditionHides, ClampMin = "1.0")) 
	float LingeringFieldRadius;

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