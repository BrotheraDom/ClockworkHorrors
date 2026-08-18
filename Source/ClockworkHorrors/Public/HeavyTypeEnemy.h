// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "HeavyTypeEnemy.generated.h"


UCLASS()
class CLOCKWORKHORRORS_API AHeavyTypeEnemy : public AEnemy
{
	GENERATED_BODY()


public:

	AHeavyTypeEnemy();


protected:

	// =========================================================
	// ATTACK
	// =========================================================

	virtual void PerformAttack() override;


	// =========================================================
	// HEAVY SETTINGS
	// =========================================================

	/**
	 * Horizontal knockback strength.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Heavy"
	)
	float KnockbackForce = 900.0f;


	/**
	 * Upward force added to the knockback.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Heavy"
	)
	float KnockbackUpForce = 250.0f;


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Enemy|Heavy"
	)
	void OnHeavyAttack();
};