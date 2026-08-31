// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/Companions/CompanionAsaila.h"

ACompanionAsaila::ACompanionAsaila()
{

}

bool ACompanionAsaila::CastSpellSlot(int spellslot, AActor* Target)
{
	UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: Attempting to cast spell slot %d on target %s"), spellslot, Target ? *Target->GetName() : TEXT("None"));
	if (SpellManager)
	{
		if (!SpellManager->AvailableSpells.IsValidIndex(spellslot))
		{
			UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: Invalid spell slot %d"), spellslot);
			return false;
		}
		if (SpellManager->CanCastSpell(SpellManager->AvailableSpells[spellslot]))
		{
			if (Target == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: Target is null, cannot cast spell slot %d"), spellslot);
				return false;
			}
			UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: Casting spell slot %d on target %s"), spellslot, *Target->GetName());
			SpellManager->CastSpell(SpellManager->AvailableSpells[spellslot], Target);
			return true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: Cannot cast spell slot %d, either on cooldown or not enough mana"), spellslot);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("ACompanionAsaila: SpellManager is null, cannot cast spell slot %d"), spellslot);
	}
	return false;
}
