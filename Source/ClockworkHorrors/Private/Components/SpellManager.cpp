// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Components/SpellManager.h"
#include "Components/ManaComponent.h"
#include "Engine/DamageEvents.h"
#include "Interfaces/PlayerInterface.h"

// Sets default values for this component's properties
USpellManager::USpellManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USpellManager::BeginPlay()
{
	Super::BeginPlay();

	ManaComponent = GetOwner()->FindComponentByClass<UManaComponent>();
}

void USpellManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateCooldowns(DeltaTime);
}

bool USpellManager::CastSpell(USpell* Spell, AActor* Target)
{
	if (!CanCastSpell(Spell))
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Cannot cast spell %s"), *Spell->SpellName.ToString());
		return false;
	}

	if (ManaComponent && !ManaComponent->ConsumeMana(Spell->ManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Not enough mana to cast spell %s"), *Spell->SpellName.ToString());
		return false;
	}

	SpellCooldowns.Add(Spell->SpellName, Spell->Cooldown);

	ExecuteSpellEffects(Spell, Target);

	OnSpellCast.Broadcast(Spell, Target);
	OnSpellCooldownUpdated.Broadcast(Spell->SpellName);

	return true;
}

bool USpellManager::CanCastSpell(USpell* Spell) const
{
	if (!Spell)
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Spell is null"));
		return false;
	}

	if (IsSpellOnCooldown(Spell))
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Spell %s is on cooldown"), *Spell->SpellName.ToString());
		return false;
	}

	if (ManaComponent && !ManaComponent->HasEnoughMana(Spell->ManaCost))
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Not enough mana to cast spell %s"), *Spell->SpellName.ToString());
		return false;
	}

	return true;
}

FSpellCooldownInfo USpellManager::GetSpellCooldownInfo(USpell* Spell) const
{
	FSpellCooldownInfo Info;

	if (Spell && SpellCooldowns.Contains(Spell->SpellName))
	{
		Info.RemainingTime = SpellCooldowns[Spell->SpellName];
		Info.bIsOnCooldown = Info.RemainingTime > 0.0f;
	}

	return Info;
}

bool USpellManager::IsSpellOnCooldown(USpell* Spell) const
{
	if (!Spell)
	{
		return true;
	}

	const float* CooldownTime = SpellCooldowns.Find(Spell->SpellName);
	return CooldownTime && *CooldownTime > 0.0f;
}

void USpellManager::UpdateCooldowns(float DeltaTime)
{
	TArray<FName> ExpiredCooldowns;

	for (auto& Pair : SpellCooldowns)
	{
		Pair.Value -= DeltaTime;

		if (Pair.Value <= 0.0f)
		{
			ExpiredCooldowns.Add(Pair.Key);
		}
	}

	for (const FName& SpellName : ExpiredCooldowns)
	{
		SpellCooldowns.Remove(SpellName);
		OnSpellCooldownUpdated.Broadcast(SpellName);
	}
}

void USpellManager::ExecuteSpellEffects(USpell* Spell, AActor* Target)
{
	if (!Spell || !Target)
	{
		return;
	}

	
	if (Spell->Damage > 0.0f)
	{
		Target->TakeDamage(Spell->Damage, FDamageEvent(), GetOwner()->GetInstigatorController(), GetOwner());
	}

	if (Spell->HealAmount > 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("USpellManager: Healing target %s for %f"), *Target->GetName(), Spell->HealAmount);
		IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(Target);
		if (PlayerInterface)
		{
			PlayerInterface->HealPlayer(Spell->HealAmount);
		}
	}

	if (Spell->CastEffect)
	{
		
	}
}