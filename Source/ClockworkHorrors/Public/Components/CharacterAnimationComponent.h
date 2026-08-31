// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Animation/CharacterAnimationSet.h"
#include "CharacterAnimationComponent.generated.h"

class UAnimMontage;
class USkeletalMeshComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnCharacterActionAnimationEnded,
    ECharacterActionAnimationType, AnimationType,
    bool, bInterrupted
);

UCLASS(ClassGroup = (Animation), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UCharacterAnimationComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UCharacterAnimationComponent();

    // =========================================================
    // ACTION ANIMATIONS
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Actions")
    bool PlayActionAnimation(
        ECharacterActionAnimationType AnimationType,
        float PlayRate = 1.0f
    );

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Actions")
    void StopActionAnimation(float BlendOutTime = 0.15f);

    UFUNCTION(BlueprintPure, Category = "Character Animation|Actions")
    bool HasActionAnimation(ECharacterActionAnimationType AnimationType) const;

    UPROPERTY(BlueprintAssignable, Category = "Character Animation|Actions")
    FOnCharacterActionAnimationEnded OnActionAnimationEnded;

    // =========================================================
    // ANIMATION SET
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Setup")
    void SetAnimationSet(UCharacterAnimationSet* NewAnimationSet);

    UFUNCTION(BlueprintPure, Category = "Character Animation|Setup")
    UCharacterAnimationSet* GetAnimationSet() const
    {
        return AnimationSet;
    }

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Setup")
    void SetAnimationMesh(USkeletalMeshComponent* NewAnimationMesh);

    // =========================================================
    // ANIMATION STANCE
    //
    // This is the persistent body/weapon posture used to choose
    // Idle / Walk / Run / Crouch / Strafe / Jump animations.
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Stance")
    void SetAnimationStance(ECharacterAnimationStance NewStance);

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Stance")
    void ResetAnimationStance();

    UFUNCTION(BlueprintPure, Category = "Character Animation|Stance")
    ECharacterAnimationStance GetAnimationStance() const
    {
        return AnimationStance;
    }

    // =========================================================
    // LOCOMOTION OVERRIDE
    // =========================================================

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Locomotion")
    void SetLocomotionOverride(ECharacterLocomotionState NewState);

    UFUNCTION(BlueprintCallable, Category = "Character Animation|Locomotion")
    void ClearLocomotionOverride();

    UFUNCTION(BlueprintPure, Category = "Character Animation|Locomotion")
    bool HasLocomotionOverride() const
    {
        return bHasLocomotionOverride;
    }

    UFUNCTION(BlueprintPure, Category = "Character Animation|Locomotion")
    ECharacterLocomotionState GetLocomotionOverride() const
    {
        return LocomotionOverride;
    }

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation|Setup")
    TObjectPtr<UCharacterAnimationSet> AnimationSet;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation|Setup")
    TObjectPtr<USkeletalMeshComponent> AnimationMesh;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Character Animation|Stance"
    )
    ECharacterAnimationStance AnimationStance =
        ECharacterAnimationStance::Unarmed;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Animation|Setup")
    FName ActionSlotName = TEXT("DefaultSlot");

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Character Animation|Setup",
        meta = (ClampMin = "0.0")
    )
    float ActionBlendInTime = 0.15f;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Character Animation|Setup",
        meta = (ClampMin = "0.0")
    )
    float ActionBlendOutTime = 0.15f;

private:
    void ResolveAnimationMesh();

    void HandleMontageEnded(UAnimMontage* Montage, bool bInterrupted);

    UPROPERTY(Transient)
    TObjectPtr<UAnimMontage> ActiveMontage;

    ECharacterActionAnimationType ActiveActionType =
        ECharacterActionAnimationType::Attack;

    bool bHasLocomotionOverride = false;

    ECharacterLocomotionState LocomotionOverride =
        ECharacterLocomotionState::Idle;
};
