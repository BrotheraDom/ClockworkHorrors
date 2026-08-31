// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "SpellProjectile.h"

#include "Spells.h"
#include "SpellLingeringField.h"
#include "BaseCharacter.h"
#include "Enemy.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);

	CollisionSphere->InitSphereRadius(20.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);
	CollisionSphere->SetGenerateOverlapEvents(true);

	ProjectileAppearance = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileAppearance"));
	ProjectileAppearance->SetupAttachment(CollisionSphere);
	ProjectileAppearance->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->Bounciness = 1.0f;
	ProjectileMovement->Friction = 0.0f;
	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 0.0f;
	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->HomingAccelerationMagnitude = 5000.0f;

	CastingActor = nullptr;
	SpellDefinition = nullptr;

	bShowProjectileHitbox = false;
	bShowLingeringHitbox = false;

	CurrentBounceCount = 0;
	CurrentPierceCount = 0;

	CurrentImpactDamage = 0.0f;
	ChargePercent = 0.0f;
}

void ASpellProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentHit.AddDynamic(this, &ASpellProjectile::ProjectileImpact);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &ASpellProjectile::ProjectileOverlap);
}

void ASpellProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SpellDefinition && SpellDefinition->bProjectileHoming)
	{
		USceneComponent* HomingTargetComponent = ProjectileMovement->HomingTargetComponent.Get();

		if (!IsValid(HomingTargetComponent) || DamagedActors.Contains(HomingTargetComponent->GetOwner()))
		{
			AcquireHomingTarget();
		}
	}

	if (!bShowProjectileHitbox || !CollisionSphere)
	{
		return;
	}

	DrawDebugSphere(
		GetWorld(),
		CollisionSphere->GetComponentLocation(),
		CollisionSphere->GetScaledSphereRadius(),
		24,
		FColor::Red,
		false,
		0.0f,
		0,
		1.5f
	);
}

void ASpellProjectile::InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection)
{
	InitializeProjectile(InCastingActor, InSpellDefinition, InTravelDirection, 0.0f);
}

void ASpellProjectile::InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection, float InChargePercent)
{
	CastingActor = InCastingActor;
	SpellDefinition = InSpellDefinition;

	if (!SpellDefinition)
	{
		Destroy();
		return;
	}

	ChargePercent = FMath::Clamp(InChargePercent, 0.0f, 1.0f);

	CurrentBounceCount = 0;
	CurrentPierceCount = 0;
	DamagedActors.Empty();

	float ProjectileSpeed = SpellDefinition->ProjectileTravelSpeed;
	float ProjectileRadius = SpellDefinition->ProjectileCollisionRadius;

	CurrentImpactDamage = SpellDefinition->ImpactDamageStrength;

	if (SpellDefinition->bChargeableSpell)
	{
		if (SpellDefinition->bChargeAffectsImpactDamage)
		{
			const float DamageMultiplier = FMath::Lerp(
				SpellDefinition->MinimumChargeDamageMultiplier,
				SpellDefinition->MaximumChargeDamageMultiplier,
				ChargePercent
			);

			CurrentImpactDamage *= DamageMultiplier;
		}

		if (SpellDefinition->bChargeAffectsProjectileSize)
		{
			const float SizeMultiplier = FMath::Lerp(
				SpellDefinition->MinimumChargeSizeMultiplier,
				SpellDefinition->MaximumChargeSizeMultiplier,
				ChargePercent
			);

			ProjectileRadius *= SizeMultiplier;

			ProjectileAppearance->SetRelativeScale3D(
				ProjectileAppearance->GetRelativeScale3D() *
				SizeMultiplier
			);
		}

		if (SpellDefinition->bChargeAffectsProjectileSpeed)
		{
			const float SpeedMultiplier = FMath::Lerp(
				SpellDefinition->MinimumChargeSpeedMultiplier,
				SpellDefinition->MaximumChargeSpeedMultiplier,
				ChargePercent
			);

			ProjectileSpeed *= SpeedMultiplier;
		}
	}

	CollisionSphere->SetSphereRadius(ProjectileRadius);

	ProjectileMovement->InitialSpeed = ProjectileSpeed;

	ProjectileMovement->MaxSpeed = ProjectileSpeed;

	ProjectileMovement->ProjectileGravityScale = SpellDefinition->ProjectileGravityStrength;

	ProjectileMovement->bShouldBounce =
		SpellDefinition->bProjectileBounces &&
		SpellDefinition->MaximumProjectileBounces > 0;

	ProjectileMovement->Bounciness =
		SpellDefinition->ProjectileBounciness;

	ProjectileMovement->Friction = 0.0f;

	ProjectileMovement->BounceVelocityStopSimulatingThreshold = 0.0f;

	if (SpellDefinition->bProjectilePierces && !SpellDefinition->bProjectileBounces)
	{
		CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	}
	else
	{
		CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}

	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->HomingTargetComponent.Reset();

	if (SpellDefinition->bProjectileHoming)
	{
		ProjectileMovement->HomingAccelerationMagnitude = SpellDefinition->ProjectileHomingStrength;

		AcquireHomingTarget();
	}

	FVector TravelDirection = InTravelDirection;

	if (TravelDirection.IsNearlyZero())
	{
		TravelDirection = GetActorForwardVector();
	}

	TravelDirection.Normalize();

	ProjectileMovement->Velocity =
		TravelDirection *
		ProjectileSpeed;

	SetLifeSpan(SpellDefinition->ProjectileExistenceSeconds);

	if (CastingActor)
	{
		CollisionSphere->IgnoreActorWhenMoving(CastingActor, true);
	}
}

void ASpellProjectile::ProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!SpellDefinition)
	{
		return;
	}

	if (!OtherActor)
	{
		return;
	}

	if (OtherActor == CastingActor)
	{
		return;
	}

	if (!SpellDefinition->bProjectilePierces)
	{
		return;
	}

	if (SpellDefinition->bProjectileBounces)
	{
		return;
	}

	const bool bNewEnemyHit = ApplyImpactDamageOnce(OtherActor);

	if (!bNewEnemyHit)
	{
		return;
	}

	CurrentPierceCount++;

	if (SpellDefinition->bModifyDamagePerPierce)
	{
		CurrentImpactDamage *= SpellDefinition->DamageMultiplierPerPierce;
	}

	if (
		SpellDefinition->bLimitProjectilePierces &&
		CurrentPierceCount >= SpellDefinition->MaximumProjectilePierces
		)
	{
		CollisionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ASpellProjectile::ProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
	{
		Destroy();
		return;
	}

	if (OtherActor == CastingActor)
	{
		return;
	}

	if (!SpellDefinition)
	{
		Destroy();
		return;
	}

	ApplyImpactDamageOnce(OtherActor);

	if (
		SpellDefinition->bProjectileBounces &&
		CurrentBounceCount < SpellDefinition->MaximumProjectileBounces
		)
	{
		CurrentBounceCount++;

		return;
	}

	if (SpellDefinition->ProducesLingeringField())
	{
		FVector FieldLocation = CollisionSphere->GetComponentLocation();

		if (Hit.bBlockingHit)
		{
			const float MaximumImpactDistance = FMath::Max(
				CollisionSphere->GetScaledSphereRadius() * 2.0f,
				100.0f
			);

			if (
				FVector::DistSquared(
					Hit.ImpactPoint,
					FieldLocation
				) <=
				FMath::Square(MaximumImpactDistance)
				)
			{
				FieldLocation = Hit.ImpactPoint;
			}
		}

		CreateLingeringField(FieldLocation);
	}

	Destroy();
}

bool ASpellProjectile::ApplyImpactDamageOnce(AActor* OtherActor)
{
	if (!SpellDefinition)
	{
		return false;
	}

	if (!OtherActor)
	{
		return false;
	}

	if (DamagedActors.Contains(OtherActor))
	{
		return false;
	}

	if (AEnemy* EnemyCharacter = Cast<AEnemy>(OtherActor))
	{
		DamagedActors.Add(OtherActor);

		if (SpellDefinition->bInflictsImpactDamage)
		{
			AController* DamageInstigator = nullptr;

			if (APawn* CastingPawn = Cast<APawn>(CastingActor))
			{
				DamageInstigator = CastingPawn->GetController();
			}

			UGameplayStatics::ApplyDamage(
				EnemyCharacter,
				CurrentImpactDamage,
				DamageInstigator,
				this,
				nullptr
			);
		}

		if (SpellDefinition->bProjectileHoming)
		{
			AcquireHomingTarget();
		}

		return true;
	}

	if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		//PlayerCharacter->ApplyDamage(SpellDefinition->ImpactDamageStrength);
	}

	return false;
}

void ASpellProjectile::AcquireHomingTarget()
{
	if (!SpellDefinition)
	{
		return;
	}

	if (!SpellDefinition->bProjectileHoming)
	{
		return;
	}

	ProjectileMovement->bIsHomingProjectile = false;
	ProjectileMovement->HomingTargetComponent.Reset();

	TArray<AActor*> EnemyActors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AEnemy::StaticClass(),
		EnemyActors
	);

	AActor* ClosestEnemy = nullptr;

	float ClosestDistanceSquared = FMath::Square(SpellDefinition->ProjectileHomingDetectionRadius);

	for (AActor* EnemyActor : EnemyActors)
	{
		if (!IsValid(EnemyActor))
		{
			continue;
		}

		if (DamagedActors.Contains(EnemyActor))
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(
			GetActorLocation(),
			EnemyActor->GetActorLocation()
		);

		if (DistanceSquared > ClosestDistanceSquared)
		{
			continue;
		}

		ClosestEnemy = EnemyActor;
		ClosestDistanceSquared = DistanceSquared;
	}

	if (!IsValid(ClosestEnemy))
	{
		return;
	}

	USceneComponent* TargetComponent = ClosestEnemy->GetRootComponent();

	if (!IsValid(TargetComponent))
	{
		return;
	}

	ProjectileMovement->HomingTargetComponent = TargetComponent;
	ProjectileMovement->bIsHomingProjectile = true;
}

void ASpellProjectile::CreateLingeringField(const FVector& FieldLocation)
{
	if (!SpellDefinition)
	{
		return;
	}

	if (!SpellDefinition->ProducesLingeringField())
	{
		return;
	}

	FActorSpawnParameters SpawnParameters;

	SpawnParameters.Owner = CastingActor;
	SpawnParameters.Instigator = Cast<APawn>(CastingActor);
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ASpellLingeringField* LingeringField = GetWorld()->SpawnActor<ASpellLingeringField>(
		ASpellLingeringField::StaticClass(),
		FieldLocation,
		FRotator::ZeroRotator,
		SpawnParameters
	);

	if (!IsValid(LingeringField))
	{
		return;
	}

	LingeringField->bShowLingeringHitbox = bShowLingeringHitbox;

	LingeringField->InitializeLingeringField(
		CastingActor,
		SpellDefinition
	);
}