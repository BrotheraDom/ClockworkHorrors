// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Animation/CharacterAnimInstance.h"

#include "Components/CharacterAnimationComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"


// =========================================================
// INITIALIZE
// =========================================================

void UCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


	APawn* PawnOwner =
		TryGetPawnOwner();


	if (!PawnOwner)
	{
		return;
	}


	AnimationComponent =
		PawnOwner->FindComponentByClass<
		UCharacterAnimationComponent
		>();


	RefreshAnimationSet();
}


// =========================================================
// UPDATE
// =========================================================

void UCharacterAnimInstance::NativeUpdateAnimation(
	float DeltaSeconds
)
{
	Super::NativeUpdateAnimation(
		DeltaSeconds
	);


	APawn* PawnOwner =
		TryGetPawnOwner();


	if (!PawnOwner)
	{
		return;
	}


	// ---------------------------------------------------------
	// FIND COMPONENT IF NEEDED
	// ---------------------------------------------------------

	if (!AnimationComponent)
	{
		AnimationComponent =
			PawnOwner->FindComponentByClass<
			UCharacterAnimationComponent
			>();
	}


	// ---------------------------------------------------------
	// DETECT ANIMATION SET CHANGES
	// ---------------------------------------------------------

	if (
		AnimationComponent &&
		AnimationSet !=
		AnimationComponent->GetAnimationSet()
		)
	{
		RefreshAnimationSet();
	}


	// ---------------------------------------------------------
	// MOVEMENT DATA
	// ---------------------------------------------------------

	FVector HorizontalVelocity =
		PawnOwner->GetVelocity();


	HorizontalVelocity.Z = 0.0f;


	GroundSpeed =
		HorizontalVelocity.Size();


	bIsMoving =
		GroundSpeed > KINDA_SMALL_NUMBER;


	const FVector LocalVelocity =
		PawnOwner
		->GetActorTransform()
		.InverseTransformVectorNoScale(
			HorizontalVelocity
		);


	if (bIsMoving)
	{
		MovementDirection =
			FMath::RadiansToDegrees(
				FMath::Atan2(
					LocalVelocity.Y,
					LocalVelocity.X
				)
			);
	}
	else
	{
		MovementDirection = 0.0f;
	}


	// ---------------------------------------------------------
	// CHARACTER-SPECIFIC MOVEMENT DATA
	// ---------------------------------------------------------

	bIsInAir = false;

	bIsCrouching = false;


	if (ACharacter* Character =
		Cast<ACharacter>(PawnOwner))
	{
		bIsCrouching =
			Character->IsCrouched();


		if (
			UCharacterMovementComponent*
			MovementComponent =
			Character->
			GetCharacterMovement()
			)
		{
			bIsInAir =
				MovementComponent->IsFalling();
		}
	}


	// =========================================================
	// SPECIAL LOCOMOTION OVERRIDE
	// =========================================================

	if (
		AnimationComponent &&
		AnimationComponent->
		HasLocomotionOverride()
		)
	{
		LocomotionState =
			AnimationComponent->
			GetLocomotionOverride();

		return;
	}


	// =========================================================
	// AUTOMATIC LOCOMOTION
	// =========================================================

	if (bIsInAir)
	{
		LocomotionState =
			ECharacterLocomotionState::Jump;

		return;
	}


	if (bIsCrouching)
	{
		LocomotionState =
			ECharacterLocomotionState::Crouch;

		return;
	}


	if (!bIsMoving)
	{
		LocomotionState =
			ECharacterLocomotionState::Idle;

		return;
	}


	// Mostly sideways movement.
	if (
		FMath::Abs(LocalVelocity.Y) >
		FMath::Abs(LocalVelocity.X)
		)
	{
		LocomotionState =
			ECharacterLocomotionState::Strafe;

		return;
	}


	if (
		GroundSpeed >=
		RunSpeedThreshold
		)
	{
		LocomotionState =
			ECharacterLocomotionState::Run;

		return;
	}


	LocomotionState =
		ECharacterLocomotionState::Walk;
}


// =========================================================
// REFRESH ANIMATION SET
// =========================================================

void UCharacterAnimInstance::RefreshAnimationSet()
{
	if (!AnimationComponent)
	{
		AnimationSet = nullptr;

		return;
	}


	AnimationSet =
		AnimationComponent->
		GetAnimationSet();


	if (!AnimationSet)
	{
		IdleAnimation = nullptr;
		WalkAnimation = nullptr;
		RunAnimation = nullptr;
		CrouchAnimation = nullptr;
		StrafeAnimation = nullptr;
		JumpAnimation = nullptr;
		RowAnimation = nullptr;
		RideAnimation = nullptr;
		ThrottleAnimation = nullptr;

		RunSpeedThreshold = 350.0f;

		return;
	}


	IdleAnimation =
		AnimationSet->IdleAnimation;

	WalkAnimation =
		AnimationSet->WalkAnimation;

	RunAnimation =
		AnimationSet->RunAnimation;

	CrouchAnimation =
		AnimationSet->CrouchAnimation;

	StrafeAnimation =
		AnimationSet->StrafeAnimation;

	JumpAnimation =
		AnimationSet->JumpAnimation;

	RowAnimation =
		AnimationSet->RowAnimation;

	RideAnimation =
		AnimationSet->RideAnimation;

	ThrottleAnimation =
		AnimationSet->ThrottleAnimation;


	RunSpeedThreshold =
		AnimationSet->RunSpeedThreshold;
}