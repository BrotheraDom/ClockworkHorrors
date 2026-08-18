// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "IceTypeEnemy.generated.h"


UCLASS()
class CLOCKWORKHORRORS_API AIceTypeEnemy : public AEnemy
{
	GENERATED_BODY()


public:

	AIceTypeEnemy();


protected:

	// =========================================================
	// ATTACK
	// =========================================================

	virtual void PerformAttack() override;


	// =========================================================
	// ICE SETTINGS
	// =========================================================

	/**
	 * Movement multiplier that will eventually be applied
	 * to the target.
	 *
	 * 1.0 = normal speed
	 * 0.5 = 50% speed
	 * 0.25 = 25% speed
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Ice",
		meta = (
			ClampMin = "0.0",
			ClampMax = "1.0"
			)
	)
	float SlowMultiplier = 0.50f;


	/**
	 * How long the ice slow should last.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Ice"
	)
	float SlowDuration = 2.5f;


	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Enemy|Ice"
	)
	void OnIceAttack();
};