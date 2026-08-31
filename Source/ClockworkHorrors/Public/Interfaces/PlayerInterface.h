// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class CLOCKWORKHORRORS_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	virtual void ResumeGame() = 0;
	virtual void OpenOptionsMenu() = 0;
	virtual void PreviousMenu() = 0;
	virtual void Respawn() = 0;
	virtual float GetCurrentHealth() const = 0;
	virtual void UpdateCompanionTarget(AActor* NewTarget, float damage) = 0;
	virtual void HealPlayer(float HealAmount) = 0;
};
