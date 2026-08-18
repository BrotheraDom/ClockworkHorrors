// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Components/CharacterAnimationComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

UCharacterAnimationComponent::UCharacterAnimationComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UCharacterAnimationComponent::BeginPlay()
{
    Super::BeginPlay();

    ResolveAnimationMesh();
}

void UCharacterAnimationComponent::ResolveAnimationMesh()
{
    if (AnimationMesh)
    {
        return;
    }

    AActor* Owner = GetOwner();

    if (!Owner)
    {
        return;
    }

    if (ACharacter* Character = Cast<ACharacter>(Owner))
    {
        AnimationMesh = Character->GetMesh();

        if (AnimationMesh)
        {
            return;
        }
    }

    AnimationMesh = Owner->FindComponentByClass<USkeletalMeshComponent>();
}

void UCharacterAnimationComponent::SetAnimationSet(
    UCharacterAnimationSet* NewAnimationSet
)
{
    AnimationSet = NewAnimationSet;
}

void UCharacterAnimationComponent::SetAnimationMesh(
    USkeletalMeshComponent* NewAnimationMesh
)
{
    AnimationMesh = NewAnimationMesh;
}

bool UCharacterAnimationComponent::HasActionAnimation(
    ECharacterActionAnimationType AnimationType
) const
{
    if (!AnimationSet)
    {
        return false;
    }

    return AnimationSet->GetActionAnimation(AnimationType) != nullptr;
}

bool UCharacterAnimationComponent::PlayActionAnimation(
    ECharacterActionAnimationType AnimationType,
    float PlayRate
)
{
    if (!AnimationSet)
    {
        return false;
    }

    UAnimSequenceBase* Animation =
        AnimationSet->GetActionAnimation(AnimationType);

    // No animation assigned is valid for this system.
    if (!Animation)
    {
        return false;
    }

    ResolveAnimationMesh();

    if (!AnimationMesh)
    {
        return false;
    }

    UAnimInstance* AnimInstance = AnimationMesh->GetAnimInstance();

    if (!AnimInstance)
    {
        return false;
    }

    const float SafePlayRate = FMath::Max(PlayRate, 0.01f);

    UAnimMontage* MontageToTrack = nullptr;

    // If the assigned asset is already a montage, play it directly.
    if (UAnimMontage* Montage = Cast<UAnimMontage>(Animation))
    {
        const float Duration = AnimInstance->Montage_Play(
            Montage,
            SafePlayRate
        );

        if (Duration <= 0.0f)
        {
            return false;
        }

        MontageToTrack = Montage;
    }
    else
    {
        // Normal animation sequences are converted to a temporary dynamic
        // montage and played through the configured slot.
        MontageToTrack = AnimInstance->PlaySlotAnimationAsDynamicMontage(
            Animation,
            ActionSlotName,
            ActionBlendInTime,
            ActionBlendOutTime,
            SafePlayRate,
            1,
            -1.0f,
            0.0f
        );

        if (!MontageToTrack)
        {
            return false;
        }
    }

    ActiveMontage = MontageToTrack;
    ActiveActionType = AnimationType;

    FOnMontageEnded EndDelegate;
    EndDelegate.BindUObject(
        this,
        &UCharacterAnimationComponent::HandleMontageEnded
    );

    AnimInstance->Montage_SetEndDelegate(
        EndDelegate,
        ActiveMontage
    );

    return true;
}

void UCharacterAnimationComponent::StopActionAnimation(float BlendOutTime)
{
    if (!AnimationMesh || !ActiveMontage)
    {
        return;
    }

    UAnimInstance* AnimInstance = AnimationMesh->GetAnimInstance();

    if (!AnimInstance)
    {
        return;
    }

    AnimInstance->Montage_Stop(
        FMath::Max(BlendOutTime, 0.0f),
        ActiveMontage
    );
}

void UCharacterAnimationComponent::HandleMontageEnded(
    UAnimMontage* Montage,
    bool bInterrupted
)
{
    if (!Montage || Montage != ActiveMontage)
    {
        return;
    }

    const ECharacterActionAnimationType FinishedAction = ActiveActionType;

    ActiveMontage = nullptr;

    OnActionAnimationEnded.Broadcast(
        FinishedAction,
        bInterrupted
    );
}

void UCharacterAnimationComponent::SetLocomotionOverride(
    ECharacterLocomotionState NewState
)
{
    LocomotionOverride = NewState;
    bHasLocomotionOverride = true;
}

void UCharacterAnimationComponent::ClearLocomotionOverride()
{
    bHasLocomotionOverride = false;
}
