// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Animation/CharacterAnimationSet.h"


// =========================================================
// GET ACTION ANIMATION
// =========================================================

UAnimSequenceBase*
UCharacterAnimationSet::GetActionAnimation(
	ECharacterActionAnimationType AnimationType
) const
{
	switch (AnimationType)
	{
	case ECharacterActionAnimationType::Dodge:
		return DodgeAnimation;

	case ECharacterActionAnimationType::Roll:
		return RollAnimation;

	case ECharacterActionAnimationType::Attack:
		return AttackAnimation;

	case ECharacterActionAnimationType::Shoot:
		return ShootAnimation;

	case ECharacterActionAnimationType::Reload:
		return ReloadAnimation;

	case ECharacterActionAnimationType::Heal:
		return HealAnimation;

	case ECharacterActionAnimationType::Craft:
		return CraftAnimation;

	case ECharacterActionAnimationType::Pickup:
		return PickupAnimation;

	case ECharacterActionAnimationType::Cast:
		return CastAnimation;

	case ECharacterActionAnimationType::Sleep:
		return SleepAnimation;

	case ECharacterActionAnimationType::Rage:
		return RageAnimation;

	default:
		return nullptr;
	}
}


// =========================================================
// GET LOCOMOTION ANIMATION
// =========================================================

UAnimSequenceBase*
UCharacterAnimationSet::GetLocomotionAnimation(
	ECharacterLocomotionState LocomotionState
) const
{
	switch (LocomotionState)
	{
	case ECharacterLocomotionState::Idle:
		return IdleAnimation;

	case ECharacterLocomotionState::Walk:
		return WalkAnimation;

	case ECharacterLocomotionState::Run:
		return RunAnimation;

	case ECharacterLocomotionState::Crouch:
		return CrouchAnimation;

	case ECharacterLocomotionState::Strafe:
		return StrafeAnimation;

	case ECharacterLocomotionState::Jump:
		return JumpAnimation;

	case ECharacterLocomotionState::Row:
		return RowAnimation;

	case ECharacterLocomotionState::Ride:
		return RideAnimation;

	case ECharacterLocomotionState::Throttle:
		return ThrottleAnimation;

	default:
		return nullptr;
	}
}