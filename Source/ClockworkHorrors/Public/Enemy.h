// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/EnemyInterface.h"
#include "Enemy.generated.h"


class UHealthComponent;
class UInputComponent;


UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Patrolling UMETA(DisplayName = "Patrolling"),
	Chasing   UMETA(DisplayName = "Chasing"),
	Attacking UMETA(DisplayName = "Attacking"),
	Dead      UMETA(DisplayName = "Dead")
};


UCLASS(Abstract)
class CLOCKWORKHORRORS_API AEnemy : public ACharacter, public IEnemyInterface
{
	GENERATED_BODY()


public:

	AEnemy();


	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(
		UInputComponent* PlayerInputComponent
	) override;

	UFUNCTION(BlueprintCallable, Category = "Enemy|Combat")
	float GetExperienceReward() const { return ExperienceReward; }


protected:

	virtual void BeginPlay() override;


	// =========================================================
	// COMPONENTS
	// =========================================================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Components"
	)
	UHealthComponent* HealthComponent;


	// =========================================================
	// COMBAT
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Combat"
	)
	float AttackDamage = 0.2f;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Combat"
	)
	float AttackRange = 150.0f;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Combat"
	)
	float AttackCooldown = 0.5f;


	// =========================================================
	// AI / MOVEMENT
	// =========================================================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|AI"
	)
	float MoveSpeed = 250.0f;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|AI"
	)
	float PatrolRadius = 800.0f;


	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|AI"
	)
	float DetectionRadius = 600.0f;


	// =========================================================
	// DEATH
	// =========================================================

	/**
	 * How long the enemy remains in the world after dying.
	 *
	 * 0 = destroy immediately.
	 *
	 * Later you can set this to something like 3 seconds
	 * if you want a death animation or ragdoll.
	 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy|Death",
		meta = (ClampMin = "0.0")
	)
	float DeathDestroyDelay = 0.0f;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|Death"
	)
	float ExperienceReward;


	// =========================================================
	// STATE
	// =========================================================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|State"
	)
	bool bIsAttacking = false;


	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|State"
	)
	bool bCanDealDamage = false;


	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|State"
	)
	EEnemyState CurrentState =
		EEnemyState::Patrolling;


	// =========================================================
	// TARGETING
	// =========================================================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|AI"
	)
	AActor* TargetActor = nullptr;


	// =========================================================
	// PATROL
	// =========================================================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|AI"
	)
	FVector SpawnLocation;


	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy|AI"
	)
	FVector PatrolDestination;


	// =========================================================
	// INTERNAL
	// =========================================================

	float AttackTimer = 0.0f;


	// =========================================================
	// SHARED ENEMY BEHAVIOR
	// =========================================================

	virtual void AcquireTarget();


	virtual void UpdateState(
		float DeltaTime
	);


	virtual void HandlePatrol(
		float DeltaTime
	);


	virtual void HandleChase(
		float DeltaTime
	);


	virtual void HandleAttack(
		float DeltaTime
	);


	virtual void ChooseNewPatrolDestination();


	/**
	 * Called when this enemy's attack cooldown finishes.
	 *
	 * Child enemy classes should override this when they
	 * need a different attack.
	 */
	virtual void PerformAttack() override;

	UFUNCTION()
	void HandleActionFinished();

	UPROPERTY(EditDefaultsOnly)
	FName ActionFinishedMessage;

	FTimerHandle AttackResetTimerHandle;


	// =========================================================
	// DEATH
	// =========================================================

	/**
	 * Called automatically when HealthComponent reaches zero.
	 */
	UFUNCTION()
	void HandleDeath();


	/**
	 * Blueprint hook for death effects.
	 *
	 * BP children can implement this for:
	 * - animation
	 * - particles
	 * - sound
	 * - ragdoll
	 * etc.
	 */
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Enemy|Events"
	)
	void OnDeath();
};