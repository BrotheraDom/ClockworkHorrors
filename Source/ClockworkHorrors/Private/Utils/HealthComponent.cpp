// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Utils/HealthComponent.h"

#include "GameFramework/Actor.h"
#include "TimerManager.h"

#if WITH_EDITOR
#include "UObject/UnrealType.h"
#endif


UHealthComponent::UHealthComponent()
{
	// Health regeneration uses timers, so this component
	// does NOT need to tick every frame.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHealth = 5.0f;

	StartingHealth = 0.0f;

	RegenerationRate = 1.0f;

	WaitTimeBeforeRegenerationRestart = 5.0f;

	HealthRegenInterval = 1.0f;

	bCanRegenerateHealth = false;

	bIsRegenerating = false;

	bDeathBroadcast = false;
}


// =========================================================
// BEGIN PLAY
// =========================================================

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();


	// ---------------------------------------------------------
	// INITIALIZE HEALTH
	// ---------------------------------------------------------

	if (StartingHealth <= 0.0f)
	{
		CurrentHealth = MaxHealth;
	}
	else
	{
		CurrentHealth = FMath::Clamp(
			StartingHealth,
			0.0f,
			MaxHealth
		);
	}


	bDeathBroadcast = false;


	// ---------------------------------------------------------
	// LISTEN FOR OWNER DAMAGE
	// ---------------------------------------------------------

	AActor* Owner = GetOwner();

	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(
			this,
			&UHealthComponent::OnOwnerTakeAnyDamage
		);
	}


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s Health initialized: %.2f / %.2f"),
		Owner ? *Owner->GetName() : TEXT("Unknown"),
		CurrentHealth,
		MaxHealth
	);
}


// =========================================================
// RECEIVE DAMAGE
// =========================================================

void UHealthComponent::OnOwnerTakeAnyDamage(
	AActor* DamagedActor,
	float Damage,
	const UDamageType* DamageType,
	AController* InstigatedBy,
	AActor* DamageCauser
)
{
	// Ignore invalid damage.
	if (Damage <= 0.0f)
	{
		return;
	}


	// Don't process damage after death.
	if (IsDead())
	{
		return;
	}


	// Damage interrupts regeneration and resets the delay.
	StopRegeneration();


	const float PreviousHealth = CurrentHealth;


	CurrentHealth = FMath::Clamp(
		CurrentHealth - Damage,
		0.0f,
		MaxHealth
	);


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s took %.2f damage. Health: %.2f -> %.2f"),
		DamagedActor ? *DamagedActor->GetName() : TEXT("Unknown"),
		Damage,
		PreviousHealth,
		CurrentHealth
	);


	// ---------------------------------------------------------
	// DEATH
	// ---------------------------------------------------------

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;

		if (!bDeathBroadcast)
		{
			bDeathBroadcast = true;


			UE_LOG(
				LogTemp,
				Log,
				TEXT("%s has died."),
				DamagedActor
				? *DamagedActor->GetName()
				: TEXT("Unknown")
			);


			OnCharacterDeath.Broadcast();
		}


		// Stop listening for additional damage once dead.
		AActor* Owner = GetOwner();

		if (Owner)
		{
			Owner->OnTakeAnyDamage.RemoveDynamic(
				this,
				&UHealthComponent::OnOwnerTakeAnyDamage
			);
		}


		return;
	}


	// ---------------------------------------------------------
	// HURT EVENT
	// ---------------------------------------------------------

	OnCharacterHurt.Broadcast();


	// ---------------------------------------------------------
	// SCHEDULE REGENERATION
	// ---------------------------------------------------------

	if (bCanRegenerateHealth)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_RegenerationRestart,
			this,
			&UHealthComponent::StartRegeneration,
			WaitTimeBeforeRegenerationRestart,
			false
		);
	}
}


// =========================================================
// HEAL
// =========================================================

void UHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f)
	{
		return;
	}


	// Healing does not resurrect dead Actors.
	if (IsDead())
	{
		return;
	}


	const float PreviousHealth = CurrentHealth;


	CurrentHealth = FMath::Clamp(
		CurrentHealth + HealAmount,
		0.0f,
		MaxHealth
	);


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s healed %.2f. Health: %.2f -> %.2f"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
		HealAmount,
		PreviousHealth,
		CurrentHealth
	);
}


// =========================================================
// START REGENERATION
// =========================================================

void UHealthComponent::StartRegeneration()
{
	if (!bCanRegenerateHealth)
	{
		return;
	}


	if (bIsRegenerating)
	{
		return;
	}


	if (IsDead())
	{
		return;
	}


	if (CurrentHealth >= MaxHealth)
	{
		return;
	}


	bIsRegenerating = true;


	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_Regeneration,
		this,
		&UHealthComponent::RegenerateHealth,
		HealthRegenInterval,
		true
	);
}


// =========================================================
// STOP REGENERATION
// =========================================================

void UHealthComponent::StopRegeneration()
{
	if (!GetWorld())
	{
		return;
	}


	FTimerManager& TimerManager =
		GetWorld()->GetTimerManager();


	TimerManager.ClearTimer(
		TimerHandle_Regeneration
	);


	TimerManager.ClearTimer(
		TimerHandle_RegenerationRestart
	);


	bIsRegenerating = false;
}


// =========================================================
// REGENERATE HEALTH
// =========================================================

void UHealthComponent::RegenerateHealth()
{
	if (IsDead())
	{
		StopRegeneration();
		return;
	}


	if (CurrentHealth >= MaxHealth)
	{
		CurrentHealth = MaxHealth;

		StopRegeneration();

		return;
	}


	Heal(RegenerationRate);


	if (CurrentHealth >= MaxHealth)
	{
		CurrentHealth = MaxHealth;

		StopRegeneration();
	}
}


// =========================================================
// EDITOR PROPERTY VALIDATION
// =========================================================

#if WITH_EDITOR

void UHealthComponent::PostEditChangeProperty(
	FPropertyChangedEvent& PropertyChangedEvent
)
{
	Super::PostEditChangeProperty(
		PropertyChangedEvent
	);


	const float MaxAllowedRegenRate =
		MaxHealth * 0.25f;


	// Don't allow regeneration to exceed
	// 25% of MaxHealth per regeneration tick.
	RegenerationRate = FMath::Clamp(
		RegenerationRate,
		0.0f,
		MaxAllowedRegenRate
	);


	// StartingHealth of zero has special meaning:
	// start at MaxHealth.
	//
	// If greater than zero, don't allow it
	// to exceed MaxHealth.
	if (StartingHealth > 0.0f)
	{
		StartingHealth = FMath::Clamp(
			StartingHealth,
			0.0f,
			MaxHealth
		);
	}
}

#endif