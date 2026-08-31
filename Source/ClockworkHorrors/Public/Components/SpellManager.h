// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utils/Spell.h"
#include "SpellManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSpellCast, USpell*, Spell, AActor*, Target);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellCooldownUpdated, FName, SpellName);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLOCKWORKHORRORS_API USpellManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellManager();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spells")
	TArray<USpell*> AvailableSpells;

	UPROPERTY(BlueprintAssignable, Category = "Spells")
	FOnSpellCast OnSpellCast;

	UPROPERTY(BlueprintAssignable, Category = "Spells")
	FOnSpellCooldownUpdated OnSpellCooldownUpdated;

	UFUNCTION(BlueprintCallable, Category = "Spells")
	bool CastSpell(USpell* Spell, AActor* Target);

	UFUNCTION(BlueprintPure, Category = "Spells")
	bool CanCastSpell(USpell* Spell) const;

	UFUNCTION(BlueprintPure, Category = "Spells")
	FSpellCooldownInfo GetSpellCooldownInfo(USpell* Spell) const;

	UFUNCTION(BlueprintPure, Category = "Spells")
	bool IsSpellOnCooldown(USpell* Spell) const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TMap<FName, float> SpellCooldowns;

	class UManaComponent* ManaComponent;

	void UpdateCooldowns(float DeltaTime);
	void ExecuteSpellEffects(USpell* Spell, AActor* Target);
};
