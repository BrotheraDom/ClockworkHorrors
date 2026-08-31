// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CompanionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCompanionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CLOCKWORKHORRORS_API ICompanionInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual bool CastSpellSlot(int spellslot, AActor* Target) = 0;
	virtual void UpdateTarget(AActor* NewTarget, float damage) = 0;
};
