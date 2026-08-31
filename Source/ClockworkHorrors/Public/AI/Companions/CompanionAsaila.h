// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AI/BaseCompanion.h"
#include "CompanionAsaila.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API ACompanionAsaila : public ABaseCompanion
{
	GENERATED_BODY()
	
public:
	ACompanionAsaila();

protected:


public:
	virtual bool CastSpellSlot(int spellslot, AActor* Target) override;
	
};
