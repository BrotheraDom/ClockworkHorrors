// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Interfaces/StatusEffectSource.h"
#include "PoisonTypeEnemy.generated.h"


UCLASS()
class CLOCKWORKHORRORS_API APoisonTypeEnemy : public AEnemy, public IStatusEffectSource
{
	GENERATED_BODY()


public:

	APoisonTypeEnemy();


protected:

	// =========================================================
	// ATTACK
	// =========================================================



	// =========================================================
	// POISON SETTINGS
	// =========================================================

	/**
	 * Damage dealt every poison tick once we add
	 * the status-effect system.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Poison"
	)
	float PoisonTickDamage = 0.05f;


	/**
	 * Time between poison damage ticks.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Poison"
	)
	float PoisonTickInterval = 1.0f;


	/**
	 * How long the poison effect lasts.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Poison"
	)
	float PoisonDuration = 5.0f;


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Enemy|Poison"
	)
	void OnPoisonAttack();

	virtual UStatusEffectType* GetStatusEffectPayload() const override;
};