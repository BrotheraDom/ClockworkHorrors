// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAnimationSet.generated.h"


class UAnimSequenceBase;


// =========================================================
// LOCOMOTION STATES
// =========================================================

UENUM(BlueprintType)
enum class ECharacterLocomotionState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Run			UMETA(DisplayName = "Run"),
	Crouch		UMETA(DisplayName = "Crouch"),
	Strafe		UMETA(DisplayName = "Strafe"),
	Jump		UMETA(DisplayName = "Jump"),
	Row			UMETA(DisplayName = "Row"),
	Ride		UMETA(DisplayName = "Ride"),
	Throttle	UMETA(DisplayName = "Throttle")
};


// =========================================================
// ACTION ANIMATIONS
// =========================================================

UENUM(BlueprintType)
enum class ECharacterActionAnimationType : uint8
{
	Dodge		UMETA(DisplayName = "Dodge"),
	Roll		UMETA(DisplayName = "Roll"),
	Attack		UMETA(DisplayName = "Attack"),
	Shoot		UMETA(DisplayName = "Shoot"),
	Reload		UMETA(DisplayName = "Reload"),
	Heal		UMETA(DisplayName = "Heal"),
	Craft		UMETA(DisplayName = "Craft"),
	Pickup		UMETA(DisplayName = "Pickup"),
	Cast		UMETA(DisplayName = "Cast"),
	Sleep		UMETA(DisplayName = "Sleep"),
	Rage		UMETA(DisplayName = "Rage")
};


// =========================================================
// CHARACTER ANIMATION SET
// =========================================================

UCLASS(BlueprintType)
class CLOCKWORKHORRORS_API UCharacterAnimationSet : public UDataAsset
{
	GENERATED_BODY()


public:

	// =========================================================
	// LOCOMOTION
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> IdleAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> WalkAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> RunAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> CrouchAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> StrafeAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion"
	)
	TObjectPtr<UAnimSequenceBase> JumpAnimation;


	// =========================================================
	// VEHICLE / SPECIAL LOCOMOTION
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion|Special"
	)
	TObjectPtr<UAnimSequenceBase> RowAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion|Special"
	)
	TObjectPtr<UAnimSequenceBase> RideAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion|Special"
	)
	TObjectPtr<UAnimSequenceBase> ThrottleAnimation;


	// =========================================================
	// COMBAT
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Combat"
	)
	TObjectPtr<UAnimSequenceBase> AttackAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Combat"
	)
	TObjectPtr<UAnimSequenceBase> ShootAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Combat"
	)
	TObjectPtr<UAnimSequenceBase> ReloadAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Combat"
	)
	TObjectPtr<UAnimSequenceBase> CastAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Combat"
	)
	TObjectPtr<UAnimSequenceBase> RageAnimation;


	// =========================================================
	// GENERAL ACTIONS
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Movement"
	)
	TObjectPtr<UAnimSequenceBase> DodgeAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Movement"
	)
	TObjectPtr<UAnimSequenceBase> RollAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Interaction"
	)
	TObjectPtr<UAnimSequenceBase> HealAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Interaction"
	)
	TObjectPtr<UAnimSequenceBase> CraftAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Interaction"
	)
	TObjectPtr<UAnimSequenceBase> PickupAnimation;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Actions|Interaction"
	)
	TObjectPtr<UAnimSequenceBase> SleepAnimation;


	// =========================================================
	// LOCOMOTION SETTINGS
	// =========================================================

	/**
	 * Speed at which locomotion changes from Walk to Run.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Locomotion|Settings",
		meta = (ClampMin = "0.0")
	)
	float RunSpeedThreshold = 350.0f;


	// =========================================================
	// HELPERS
	// =========================================================

	UFUNCTION(
		BlueprintPure,
		Category = "Animation"
	)
	UAnimSequenceBase* GetActionAnimation(
		ECharacterActionAnimationType AnimationType
	) const;


	UFUNCTION(
		BlueprintPure,
		Category = "Animation"
	)
	UAnimSequenceBase* GetLocomotionAnimation(
		ECharacterLocomotionState LocomotionState
	) const;
};