// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Spell.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSpellCooldownInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	float RemainingTime = 0.0f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsOnCooldown = false;
};

UCLASS()
class CLOCKWORKHORRORS_API USpell : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	FName SpellName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float ManaCost = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float Cooldown = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float CastTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell")
	float Range = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell|Effects")
	float Damage = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell|Effects")
	float HealAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell|Visual")
	UAnimMontage* CastAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spell|Visual")
	UParticleSystem* CastEffect;
};
