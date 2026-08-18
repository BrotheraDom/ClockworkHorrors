// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "SpellProjectile.h"

#include "Spells.h"
#include "BaseCharacter.h"
#include "Enemy.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASpellProjectile::ASpellProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	SetRootComponent(CollisionSphere);

	CollisionSphere->InitSphereRadius(20.0f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetNotifyRigidBodyCollision(true);

	ProjectileAppearance = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileAppearance"));
	ProjectileAppearance->SetupAttachment(CollisionSphere);
	ProjectileAppearance->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionSphere;
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;

	CastingActor = nullptr;
	SpellDefinition = nullptr;
}

void ASpellProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionSphere->OnComponentHit.AddDynamic(this, &ASpellProjectile::ProjectileImpact);
}

void ASpellProjectile::InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection)
{
	CastingActor = InCastingActor;
	SpellDefinition = InSpellDefinition;

	if (!SpellDefinition)
	{
		Destroy();
		return;
	}

	CollisionSphere->SetSphereRadius(SpellDefinition->ProjectileCollisionRadius);

	ProjectileMovement->InitialSpeed = SpellDefinition->ProjectileTravelSpeed;

	ProjectileMovement->MaxSpeed = SpellDefinition->ProjectileTravelSpeed;

	ProjectileMovement->ProjectileGravityScale = SpellDefinition->ProjectileGravityStrength;

	FVector TravelDirection = InTravelDirection;

	if (TravelDirection.IsNearlyZero())
	{
		TravelDirection = GetActorForwardVector();
	}

	TravelDirection.Normalize();

	ProjectileMovement->Velocity = TravelDirection * SpellDefinition->ProjectileTravelSpeed;

	SetLifeSpan(SpellDefinition->ProjectileExistenceSeconds);

	if (CastingActor)
	{
		CollisionSphere->IgnoreActorWhenMoving(CastingActor, true);
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

	if (!SpellDefinition->bInflictsImpactDamage)
	{
		Destroy();
		return;
	}

	if (AEnemy* EnemyCharacter = Cast<AEnemy>(OtherActor))
	{
		AController* DamageInstigator = nullptr;

		if (APawn* CastingPawn = Cast<APawn>(CastingActor))
		{
			DamageInstigator = CastingPawn->GetController();
		}

		UGameplayStatics::ApplyDamage(EnemyCharacter, SpellDefinition->ImpactDamageStrength, DamageInstigator, this, nullptr);

		Destroy();
		return;
	}

	if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		//PlayerCharacter->ApplyDamage(SpellDefinition->ImpactDamageStrength);

		Destroy();
		return;
	}

	Destroy();
}