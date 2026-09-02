// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Utils/HealthComponent.h"

#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "Enemy.h"
#include "BaseCharacter.h"
#include "Utils/ExperienceComponent.h"
#include "Interfaces/CompanionInterface.h"
#include "Interfaces/PlayerInterface.h"
#include "Utils/StatusEffectType.h"
#include "Interfaces/StatusEffectSource.h"


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

	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(DamagedActor);
	if (PlayerInterface)
	{
		PlayerInterface->UpdateCompanionTarget(DamageCauser, Damage);
	}
	else {
		ICompanionInterface* CompanionInterface = Cast<ICompanionInterface>(DamagedActor);
		if (CompanionInterface)
		{
			CompanionInterface->UpdateTarget(DamageCauser, Damage);
		}
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

		IStatusEffectSource* Source = Cast<IStatusEffectSource>(DamageCauser);
		if (Source)
		{
			UStatusEffectType* Payload = Source->GetStatusEffectPayload();
			if (Payload && Payload->Effect != STATUSEFFECT::None)
			{
				ApplyStatusEffect(*Payload);
			}
		}
	

	// ---------------------------------------------------------
	// DEATH
	// ---------------------------------------------------------

	if (CurrentHealth <= 0.0f)
	{
		CurrentHealth = 0.0f;

		///REWARD EXPERIENCE FOR PLAYER
		APlayerController* PlayerController = Cast<APlayerController>(InstigatedBy);
		AEnemy* Enemy = GetOwner<AEnemy>();
		if (PlayerController && Enemy)
		{
			ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(PlayerController->GetPawn());
			if(!PlayerCharacter)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s could not cast InstigatedBy's pawn to ABaseCharacter."), *GetName());
			}
			UExperienceComponent* PlayerExperienceComponent = PlayerCharacter ? PlayerCharacter->FindComponentByClass<UExperienceComponent>() : nullptr;
			if(!PlayerExperienceComponent)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s could not find UExperienceComponent on InstigatedBy's pawn."), *GetName());
			}

			PlayerExperienceComponent->AddExperience(Enemy->GetExperienceReward());
		}

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

bool UHealthComponent::CanHeal() const
{
	if(CurrentHealth < MaxHealth && !IsDead())
	{
		return true;
	}

	return false;
}

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

	OnCharacterHealed.Broadcast();

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
// RESET HEALTH
// =========================================================

void UHealthComponent::ResetHealth()
{
	CurrentHealth = MaxHealth;
	bDeathBroadcast = false;

	AActor* Owner = GetOwner();

	if (Owner)
	{
		Owner->OnTakeAnyDamage.AddDynamic(
			this,
			&UHealthComponent::OnOwnerTakeAnyDamage
		);
	}
	OnCharacterHealed.Broadcast();
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

void UHealthComponent::ApplyStatusEffect(const UStatusEffectType& Payload)
{
	switch (Payload.Effect)
	{
	case STATUSEFFECT::Poisoned:
		StartPoisonEffect(Payload.TickDamage, Payload.TickInterval, Payload.Duration);
		break;

	case STATUSEFFECT::Burning:
		StartBurningEffect(Payload.TickDamage, Payload.TickInterval, Payload.Duration);
		break;

	case STATUSEFFECT::Stunned:
		StunDuration = FMath::Max(StunDuration, Payload.Duration);
		break;

	case STATUSEFFECT::Slowed:
		SlowDuration = FMath::Max(SlowDuration, Payload.Duration);
		SlowPercentage = FMath::Max(SlowPercentage, Payload.Percentage);
		break;

	case STATUSEFFECT::Weakened:
		WeakenDuration = FMath::Max(WeakenDuration, Payload.Duration);
		WeakenPercentage = FMath::Max(WeakenPercentage, Payload.Percentage);
		break;

	default:
		break;
	}
}

void UHealthComponent::StartPoisonEffect(float TickDamage, float TickInterval, float Duration)
{
	if (TickDamage <= 0.0f || Duration <= 0.0f)
	{
		return;
	}

	PoisonTickDamage = TickDamage;
	PoisonTickInterval = FMath::Max(0.05f, TickInterval);
	PoisonRemainingDuration = Duration;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PoisonTick);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PoisonTick, this, &UHealthComponent::OnPoisonTick, PoisonTickInterval, true);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PoisonDuration);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_PoisonDuration, this, &UHealthComponent::StopPoisonEffect, Duration, false);
	}
}

void UHealthComponent::StopPoisonEffect()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PoisonTick);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_PoisonDuration);
	}

	PoisonTickDamage = 0.0f;
	PoisonRemainingDuration = 0.0f;
}

void UHealthComponent::OnPoisonTick()
{
	if (IsDead())
	{
		StopPoisonEffect();
		return;
	}

	if (PoisonTickDamage > 0.0f)
	{
		const float Prev = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth - PoisonTickDamage, 0.0f, MaxHealth);

		UE_LOG(LogTemp, Log, TEXT("%s poisoned for %.2f. Health: %.2f -> %.2f"),
			GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
			PoisonTickDamage, Prev, CurrentHealth);

		OnCharacterHurt.Broadcast();

		if (CurrentHealth <= 0.0f)
		{
			StopPoisonEffect();
		}
	}
}

void UHealthComponent::StartBurningEffect(float TickDamage, float TickInterval, float Duration)
{
	if (TickDamage <= 0.0f || Duration <= 0.0f)
	{
		return;
	}

	BurningTickDamage = TickDamage;
	BurningTickInterval = FMath::Max(0.05f, TickInterval);
	BurningRemainingDuration = Duration;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BurningTick);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_BurningTick, this, &UHealthComponent::OnBurningTick, BurningTickInterval, true);

		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BurningDuration);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_BurningDuration, this, &UHealthComponent::StopBurningEffect, Duration, false);
	}
}

void UHealthComponent::StopBurningEffect()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BurningTick);
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_BurningDuration);
	}

	BurningTickDamage = 0.0f;
	BurningRemainingDuration = 0.0f;
}

void UHealthComponent::OnBurningTick()
{
	if (IsDead())
	{
		StopBurningEffect();
		return;
	}

	if (BurningTickDamage > 0.0f)
	{
		const float Prev = CurrentHealth;
		CurrentHealth = FMath::Clamp(CurrentHealth - BurningTickDamage, 0.0f, MaxHealth);

		UE_LOG(LogTemp, Log, TEXT("%s burning for %.2f. Health: %.2f -> %.2f"),
			GetOwner() ? *GetOwner()->GetName() : TEXT("Unknown"),
			BurningTickDamage, Prev, CurrentHealth);

		OnCharacterHurt.Broadcast();

		if (CurrentHealth <= 0.0f)
		{
			StopBurningEffect();
		}
	}
}


#endif