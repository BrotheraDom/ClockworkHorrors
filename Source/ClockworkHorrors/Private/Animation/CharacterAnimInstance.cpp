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

	APawn* PawnOwner = TryGetPawnOwner();

	if (!PawnOwner)
	{
		return;
	}

	AnimationComponent =
		PawnOwner->FindComponentByClass<UCharacterAnimationComponent>();

	RefreshAnimationSet();
}


// =========================================================
// UPDATE
// =========================================================

void UCharacterAnimInstance::NativeUpdateAnimation(
	float DeltaSeconds
)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* PawnOwner = TryGetPawnOwner();

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
			PawnOwner->FindComponentByClass<UCharacterAnimationComponent>();
	}

	// ---------------------------------------------------------
	// DETECT ANIMATION SET OR STANCE CHANGES
	// ---------------------------------------------------------

	if (
		AnimationComponent &&
		(
			AnimationSet != AnimationComponent->GetAnimationSet() ||
			AnimationStance != AnimationComponent->GetAnimationStance()
			)
		)
	{
		RefreshAnimationSet();
	}

	// ---------------------------------------------------------
	// MOVEMENT DATA
	// ---------------------------------------------------------

	FVector HorizontalVelocity = PawnOwner->GetVelocity();
	HorizontalVelocity.Z = 0.0f;

	GroundSpeed = HorizontalVelocity.Size();
	bIsMoving = GroundSpeed > KINDA_SMALL_NUMBER;

	const FVector LocalVelocity =
		PawnOwner->GetActorTransform().InverseTransformVectorNoScale(
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

	if (ACharacter* Character = Cast<ACharacter>(PawnOwner))
	{
		bIsCrouching = Character->IsCrouched();

		if (
			UCharacterMovementComponent* MovementComponent =
			Character->GetCharacterMovement()
			)
		{
			bIsInAir = MovementComponent->IsFalling();
		}
	}

	// =========================================================
	// SPECIAL LOCOMOTION OVERRIDE
	// =========================================================

	if (
		AnimationComponent &&
		AnimationComponent->HasLocomotionOverride()
		)
	{
		LocomotionState = AnimationComponent->GetLocomotionOverride();
		return;
	}

	// =========================================================
	// AUTOMATIC LOCOMOTION
	// =========================================================

	if (bIsInAir)
	{
		LocomotionState = ECharacterLocomotionState::Jump;
		return;
	}

	if (bIsCrouching)
	{
		LocomotionState = ECharacterLocomotionState::Crouch;
		return;
	}

	if (!bIsMoving)
	{
		LocomotionState = ECharacterLocomotionState::Idle;
		return;
	}

	// Mostly sideways movement.
	if (
		FMath::Abs(LocalVelocity.Y) >
		FMath::Abs(LocalVelocity.X)
		)
	{
		LocomotionState = ECharacterLocomotionState::Strafe;
		return;
	}

	if (GroundSpeed >= RunSpeedThreshold)
	{
		LocomotionState = ECharacterLocomotionState::Run;
		return;
	}

	LocomotionState = ECharacterLocomotionState::Walk;
}


// =========================================================
// REFRESH ANIMATION SET / STANCE
// =========================================================

void UCharacterAnimInstance::RefreshAnimationSet()
{
	if (!AnimationComponent)
	{
		AnimationSet = nullptr;
		AnimationStance = ECharacterAnimationStance::Unarmed;
		return;
	}

	AnimationSet = AnimationComponent->GetAnimationSet();
	AnimationStance = AnimationComponent->GetAnimationStance();

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

	IdleAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Idle,
		AnimationStance
	);

	WalkAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Walk,
		AnimationStance
	);

	RunAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Run,
		AnimationStance
	);

	CrouchAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Crouch,
		AnimationStance
	);

	StrafeAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Strafe,
		AnimationStance
	);

	JumpAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Jump,
		AnimationStance
	);

	RowAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Row,
		AnimationStance
	);

	RideAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Ride,
		AnimationStance
	);

	ThrottleAnimation = AnimationSet->GetLocomotionAnimation(
		ECharacterLocomotionState::Throttle,
		AnimationStance
	);

	RunSpeedThreshold = AnimationSet->RunSpeedThreshold;
}
