// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Targeting/TargetableComponent.h"

UTargetableComponent::UTargetableComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UTargetableComponent::SetCanBeTargeted(bool bNewCanBeTargeted)
{
    bCanBeTargeted = bNewCanBeTargeted;
}

FVector UTargetableComponent::GetTargetLocation() const
{
    const AActor* Owner = GetOwner();
    if (!Owner)
    {
        return FVector::ZeroVector;
    }

    return Owner->GetActorLocation()
        + Owner->GetActorTransform().TransformVectorNoScale(TargetOffset);
}
