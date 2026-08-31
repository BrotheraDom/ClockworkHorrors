// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Components/ManaComponent.h"

// Sets default values for this component's properties
UManaComponent::UManaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UManaComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentMana = MaxMana;
}

void UManaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentMana < MaxMana)
	{
		RegenerateMana(ManaRegenRate * DeltaTime);
	}
}

bool UManaComponent::ConsumeMana(float Amount)
{
	if (!HasEnoughMana(Amount))
	{
		return false;
	}

	CurrentMana = FMath::Clamp(CurrentMana - Amount, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	return true;
}

void UManaComponent::RegenerateMana(float Amount)
{
	float OldMana = CurrentMana;
	CurrentMana = FMath::Clamp(CurrentMana + Amount, 0.0f, MaxMana);

	if (OldMana != CurrentMana)
	{
		OnManaChanged.Broadcast(CurrentMana, MaxMana);
	}
}

bool UManaComponent::HasEnoughMana(float Amount) const
{
	return CurrentMana >= Amount;
}

float UManaComponent::GetManaPercentage() const
{
	return MaxMana > 0.0f ? CurrentMana / MaxMana : 0.0f;
}

