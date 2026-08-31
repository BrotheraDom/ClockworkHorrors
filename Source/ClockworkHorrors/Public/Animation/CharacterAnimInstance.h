// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/CharacterAnimationSet.h"
#include "CharacterAnimInstance.generated.h"

class UAnimSequenceBase;
class UCharacterAnimationComponent;


// =========================================================
// UNIVERSAL CHARACTER ANIM INSTANCE
// =========================================================

UCLASS()
class CLOCKWORKHORRORS_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(
		float DeltaSeconds
	) override;

protected:

	// =========================================================
	// CURRENT STATE
	// =========================================================

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	ECharacterLocomotionState LocomotionState =
		ECharacterLocomotionState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	ECharacterAnimationStance AnimationStance =
		ECharacterAnimationStance::Unarmed;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	float GroundSpeed = 0.0f;

	/**
	 * -180 to 180 degrees.
	 * Useful later for directional strafing.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	float MovementDirection = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	bool bIsMoving = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	bool bIsInAir = false;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|State")
	bool bIsCrouching = false;


	// =========================================================
	// DATA
	// =========================================================

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Data")
	TObjectPtr<UCharacterAnimationSet> AnimationSet;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Data")
	float RunSpeedThreshold = 350.0f;


	// =========================================================
	// LOCOMOTION ANIMATION REFERENCES
	//
	// These are exposed directly so Sequence Player nodes can
	// keep using dynamic assets. Their values are resolved from
	// the current stance by C++.
	// =========================================================

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> IdleAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> WalkAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> RunAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> CrouchAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> StrafeAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> JumpAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> RowAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> RideAnimation;

	UPROPERTY(BlueprintReadOnly, Category = "Character Animation|Locomotion")
	TObjectPtr<UAnimSequenceBase> ThrottleAnimation;

private:
	void RefreshAnimationSet();

	UPROPERTY(Transient)
	TObjectPtr<UCharacterAnimationComponent> AnimationComponent;
};
