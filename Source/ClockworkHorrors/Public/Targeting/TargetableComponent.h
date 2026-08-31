// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetableComponent.generated.h"

/**
 * Marks an actor as a valid lock-on target.
 *
 * Add this component to an enemy Blueprint (or any other actor) that should
 * be targetable by the Z-lock system. No changes to AEnemy are required.
 */
UCLASS(ClassGroup = (Targeting), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UTargetableComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTargetableComponent();

    /** Can this actor currently be selected/kept as a lock-on target? */
    UFUNCTION(BlueprintPure, Category = "Targeting")
    bool CanBeTargeted() const { return bCanBeTargeted; }

    /** Enable/disable targeting at runtime (useful for death, cutscenes, etc.). */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void SetCanBeTargeted(bool bNewCanBeTargeted);

    /** World-space point the camera/lock system should aim toward. */
    UFUNCTION(BlueprintPure, Category = "Targeting")
    FVector GetTargetLocation() const;

    /** Extra score added when choosing between otherwise valid targets. */
    UFUNCTION(BlueprintPure, Category = "Targeting")
    float GetTargetPriority() const { return TargetPriority; }

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    bool bCanBeTargeted = true;

    /**
     * Local-space offset from the owning actor's location.
     * For a normal humanoid enemy, Z = 80-120 is a useful starting point.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    FVector TargetOffset = FVector(0.0f, 0.0f, 90.0f);

    /** Higher values make this target more desirable during initial selection. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
    float TargetPriority = 0.0f;
};
