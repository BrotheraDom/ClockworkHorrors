// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Enemy.h"

#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NavigationSystem.h"
#include "Engine/DamageEvents.h"
#include "Utils/HealthComponent.h"


AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;


	HealthComponent =
		CreateDefaultSubobject<UHealthComponent>(
			TEXT("HealthComponent")
		);


	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}
}


// =========================================================
// BEGIN PLAY
// =========================================================

void AEnemy::BeginPlay()
{
	Super::BeginPlay();


	SpawnLocation =
		GetActorLocation();


	AttackTimer = 0.0f;


	// ---------------------------------------------------------
	// HEALTH / DEATH
	// ---------------------------------------------------------

	if (HealthComponent)
	{
		HealthComponent->OnCharacterDeath.AddDynamic(
			this,
			&AEnemy::HandleDeath
		);
	}


	// ---------------------------------------------------------
	// AI
	// ---------------------------------------------------------

	AcquireTarget();

	ChooseNewPatrolDestination();
}


// =========================================================
// TICK
// =========================================================

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	// Dead enemies do not continue updating AI.
	if (CurrentState == EEnemyState::Dead)
	{
		return;
	}


	UpdateState(DeltaTime);
}


// =========================================================
// INPUT
// =========================================================

void AEnemy::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent
)
{
	Super::SetupPlayerInputComponent(
		PlayerInputComponent
	);
}


// =========================================================
// TARGETING
// =========================================================

void AEnemy::AcquireTarget()
{
	APawn* PlayerPawn =
		UGameplayStatics::GetPlayerPawn(
			this,
			0
		);


	if (PlayerPawn)
	{
		TargetActor = PlayerPawn;
	}
}


// =========================================================
// STATE MACHINE
// =========================================================

void AEnemy::UpdateState(float DeltaTime)
{
	if (!TargetActor)
	{
		AcquireTarget();
	}


	bIsAttacking = false;

	bCanDealDamage = false;


	// ---------------------------------------------------------
	// NO TARGET
	// ---------------------------------------------------------

	if (!TargetActor)
	{
		CurrentState =
			EEnemyState::Patrolling;


		HandlePatrol(
			DeltaTime
		);


		return;
	}


	const float DistanceToPlayer =
		FVector::Dist(
			GetActorLocation(),
			TargetActor->GetActorLocation()
		);


	// ---------------------------------------------------------
	// PATROL
	// ---------------------------------------------------------

	if (DistanceToPlayer > DetectionRadius)
	{
		CurrentState =
			EEnemyState::Patrolling;


		HandlePatrol(
			DeltaTime
		);
	}


	// ---------------------------------------------------------
	// CHASE
	// ---------------------------------------------------------

	else if (DistanceToPlayer > AttackRange)
	{
		CurrentState =
			EEnemyState::Chasing;


		HandleChase(
			DeltaTime
		);
	}


	// ---------------------------------------------------------
	// ATTACK
	// ---------------------------------------------------------

	else
	{
		CurrentState =
			EEnemyState::Attacking;


		HandleAttack(
			DeltaTime
		);
	}
}


// =========================================================
// PATROL
// =========================================================

void AEnemy::HandlePatrol(float DeltaTime)
{
	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->SetMovementMode(
			MOVE_Walking
		);


		MoveComp->MaxWalkSpeed =
			MoveSpeed;
	}


	const FVector MyLocation =
		GetActorLocation();


	FVector ToDestination =
		PatrolDestination -
		MyLocation;


	ToDestination.Z = 0.0f;


	const float DistanceToDestination =
		ToDestination.Size();


	const float AcceptRadius =
		100.0f;


	if (DistanceToDestination < AcceptRadius)
	{
		ChooseNewPatrolDestination();

		return;
	}


	ToDestination.Normalize();


	const FRotator LookAtRotation =
		FRotationMatrix::MakeFromX(
			ToDestination
		).Rotator();


	SetActorRotation(
		FRotator(
			0.0f,
			LookAtRotation.Yaw,
			0.0f
		)
	);


	AddMovementInput(
		ToDestination,
		1.0f
	);
}


// =========================================================
// CHASE
// =========================================================

void AEnemy::HandleChase(float DeltaTime)
{
	if (!TargetActor)
	{
		return;
	}


	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->SetMovementMode(
			MOVE_Walking
		);


		MoveComp->MaxWalkSpeed =
			MoveSpeed;
	}


	const FVector MyLocation =
		GetActorLocation();


	const FVector TargetLocation =
		TargetActor->GetActorLocation();


	FVector ToTarget =
		TargetLocation -
		MyLocation;


	ToTarget.Z = 0.0f;


	const float Distance =
		ToTarget.Size();


	if (Distance <= KINDA_SMALL_NUMBER)
	{
		return;
	}


	ToTarget.Normalize();


	const FRotator LookAtRotation =
		FRotationMatrix::MakeFromX(
			ToTarget
		).Rotator();


	SetActorRotation(
		FRotator(
			0.0f,
			LookAtRotation.Yaw,
			0.0f
		)
	);


	AddMovementInput(
		ToTarget,
		1.0f
	);
}


// =========================================================
// ATTACK
// =========================================================

void AEnemy::HandleAttack(float DeltaTime)
{
	if (!TargetActor)
	{
		return;
	}


	const FVector MyLocation =
		GetActorLocation();


	const FVector TargetLocation =
		TargetActor->GetActorLocation();


	FVector ToTarget =
		TargetLocation -
		MyLocation;


	ToTarget.Z = 0.0f;


	const float DistanceToPlayer =
		ToTarget.Size();


	// ---------------------------------------------------------
	// PLAYER LEFT ATTACK RANGE
	// ---------------------------------------------------------

	if (DistanceToPlayer > AttackRange)
	{
		CurrentState =
			EEnemyState::Chasing;


		HandleChase(
			DeltaTime
		);


		return;
	}


	// ---------------------------------------------------------
	// FACE PLAYER
	// ---------------------------------------------------------

	if (!ToTarget.IsNearlyZero())
	{
		const FRotator LookAtRotation =
			FRotationMatrix::MakeFromX(
				ToTarget
			).Rotator();


		SetActorRotation(
			FRotator(
				0.0f,
				LookAtRotation.Yaw,
				0.0f
			)
		);
	}


	// ---------------------------------------------------------
	// STOP MOVING WHILE ATTACKING
	// ---------------------------------------------------------

	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();

		MoveComp->DisableMovement();
	}


	bIsAttacking = true;


	// ---------------------------------------------------------
	// COOLDOWN
	// ---------------------------------------------------------

	AttackTimer -= DeltaTime;


	if (AttackTimer <= 0.0f)
	{
		PerformAttack();


		AttackTimer =
			AttackCooldown;
	}
}


// =========================================================
// DEFAULT ATTACK
// =========================================================

void AEnemy::PerformAttack()
{
	if (!TargetActor)
	{
		return;
	}


	ABaseCharacter* PlayerCharacter =
		Cast<ABaseCharacter>(
			TargetActor
		);


	if (!PlayerCharacter)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"%s could not cast TargetActor "
				"to ABaseCharacter."
			),
			*GetName()
		);


		return;
	}


	bCanDealDamage = true;


	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"%s attacked player for %.2f damage."
		),
		*GetName(),
		AttackDamage
	);


	// TakeDamage triggers the player's normal Unreal
	// damage pipeline.
	PlayerCharacter->TakeDamage(
		AttackDamage,
		FDamageEvent(),
		GetController(),
		this
	);
}


// =========================================================
// DEATH
// =========================================================

void AEnemy::HandleDeath()
{
	// Prevent death from being processed more than once.
	if (CurrentState == EEnemyState::Dead)
	{
		return;
	}


	CurrentState =
		EEnemyState::Dead;


	bIsAttacking = false;

	bCanDealDamage = false;


	TargetActor = nullptr;


	// ---------------------------------------------------------
	// STOP MOVEMENT
	// ---------------------------------------------------------

	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();

		MoveComp->DisableMovement();
	}


	// Stop running the enemy AI.
	SetActorTickEnabled(false);


	// Stop the dead enemy from blocking the player.
	SetActorEnableCollision(false);


	UE_LOG(
		LogTemp,
		Log,
		TEXT("%s died."),
		*GetName()
	);


	// Give Blueprint children an opportunity to play
	// death sounds, particles, animation, etc.
	OnDeath();


	// ---------------------------------------------------------
	// DESTROY
	// ---------------------------------------------------------

	if (DeathDestroyDelay > 0.0f)
	{
		SetLifeSpan(
			DeathDestroyDelay
		);
	}
	else
	{
		Destroy();
	}
}


// =========================================================
// PATROL DESTINATION
// =========================================================

void AEnemy::ChooseNewPatrolDestination()
{
	UNavigationSystemV1* NavSystem =
		UNavigationSystemV1::GetCurrent(
			GetWorld()
		);


	if (!NavSystem)
	{
		PatrolDestination =
			SpawnLocation;


		return;
	}


	FNavLocation RandomPoint;


	const float InnerRadius =
		FMath::Max(
			PatrolRadius - 150.0f,
			200.0f
		);


	const float UseRadius =
		FMath::Clamp(
			InnerRadius,
			200.0f,
			PatrolRadius
		);


	bool bFound = false;

	int32 Tries = 0;


	while (!bFound && Tries < 5)
	{
		++Tries;


		bFound =
			NavSystem->GetRandomReachablePointInRadius(
				SpawnLocation,
				UseRadius,
				RandomPoint
			);


		if (!bFound)
		{
			continue;
		}


		FVector Candidate =
			RandomPoint.Location;


		Candidate.Z =
			SpawnLocation.Z;


		PatrolDestination =
			Candidate;
	}


	if (!bFound)
	{
		PatrolDestination =
			SpawnLocation;
	}
}