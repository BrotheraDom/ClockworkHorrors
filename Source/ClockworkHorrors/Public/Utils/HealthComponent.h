// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Delegates/DelegateCombinations.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterHurt);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterHealed);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLOCKWORKHORRORS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	// =========================================================
	// DAMAGE
	// =========================================================

	/**
	 * Automatically called when the owning Actor receives
	 * Unreal damage through TakeDamage / ApplyDamage.
	 */

	UFUNCTION(BlueprintCallable, Category = "Health")  // Must match OnTakeAnyDamage signature exactly
	void OnOwnerTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);


	// =========================================================
	// HEALTH FUNCTIONS
	// =========================================================

	UFUNCTION()
	bool CanHeal() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }
	void SetCurrentHealth(float health) { CurrentHealth = health; }
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const
	{
		return CurrentHealth <= 0.0f;
	}

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const
	{
		if (MaxHealth <= 0.0f)
		{
			return 0.0f;
		}

		return CurrentHealth / MaxHealth;
	}

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();


	// =========================================================
	// DELEGATES
	// =========================================================

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCharacterDeath OnCharacterDeath;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCharacterHurt OnCharacterHurt;

	UPROPERTY(BlueprintAssignable, Category = "Delegates")
	FOnCharacterHealed OnCharacterHealed;

protected:


	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif



	// =========================================================
	// HEALTH SETTINGS
	// =========================================================

	UPROPERTY(
		EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0.01")
	)
	float MaxHealth = 5.0f;

	/**
	 * Health the actor begins with.
	 *
	 * A value of 0 means:
	 * Start at MaxHealth.
	 *
	 * This lets you change MaxHealth without also having
	 * to manually update StartingHealth.
	 */
	UPROPERTY(
		EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (ClampMin = "0.0")
	)

	float StartingHealth = 0.0f;

	// =========================================================
	// REGENERATION
	// =========================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health|Regeneration")
	bool bCanRegenerateHealth = false;

	/**
	 * Amount of health restored every regeneration interval.
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regeneration")
	float RegenerationRate = 1.0f;

	/**
	 * How long after receiving damage before regeneration begins.
	 */

	UPROPERTY(EditAnywhere, Category = "Health|Regeneration")
	float WaitTimeBeforeRegenerationRestart = 5.0f;


	/**
	 * Time between regeneration ticks.
	 */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regeneration")
	float HealthRegenInterval = 1.0f;



	UFUNCTION(BlueprintCallable, Category = "Health|Regeneration")
	void StartRegeneration();

	UFUNCTION(BlueprintCallable, Category = "Health|Regeneration")
	void StopRegeneration();

	UFUNCTION()
	void RegenerateHealth();


	


	

	

	

private:

	// =========================================================
	// RUNTIME HEALTH
	// =========================================================

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth = 0.0f;

	bool bIsRegenerating = false;

	bool bDeathBroadcast = false;

	//float PreviousMaxHealth;


	// =========================================================
	// TIMERS
	// =========================================================

		/** Repeating regeneration timer */
	FTimerHandle TimerHandle_Regeneration;

		/** Delay before regeneration starts again */
	FTimerHandle TimerHandle_RegenerationRestart;



		
};
