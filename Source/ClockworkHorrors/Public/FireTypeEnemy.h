// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "FireTypeEnemy.generated.h"


UCLASS()
class CLOCKWORKHORRORS_API AFireTypeEnemy : public AEnemy
{
	GENERATED_BODY()


public:

	AFireTypeEnemy();


protected:

	// =========================================================
	// ATTACK
	// =========================================================

	virtual void PerformAttack() override;


	/**
	 * Additional fire damage that we will eventually use
	 * for a burn/status-effect system.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Fire"
	)
	float BurnDamage = 0.05f;


	/**
	 * How long the burn effect should last.
	 *
	 * Currently stored here for the future status-effect
	 * system.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Fire"
	)
	float BurnDuration = 3.0f;


	/**
	 * Blueprint hook for fire-specific visuals and sound.
	 *
	 * Examples:
	 * - muzzle flash
	 * - flame burst
	 * - fire sound
	 * - fire particles on the target
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Enemy|Fire"
	)
	void OnFireAttack();
};