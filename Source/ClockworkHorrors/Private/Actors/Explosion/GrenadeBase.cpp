// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/Explosion/GrenadeBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AGrenadeBase::AGrenadeBase()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;

	CollisionComponent->InitSphereRadius(12.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetSimulatePhysics(true);
	CollisionComponent->SetEnableGravity(true);

	GrenadeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GrenadeMesh"));
	GrenadeMesh->SetupAttachment(CollisionComponent);
	GrenadeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ThrowForce = 1500.0f;
	FuseTime = 3.0f;
	ExplosionDamage = 100.0f;
	ExplosionRadius = 350.0f;

	bUseFuseTimer = true;
	bHasExploded = false;

}

// Called when the game starts or when spawned
void AGrenadeBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (bUseFuseTimer)
	{
		StartFuse();
	}
}

void AGrenadeBase::StartFuse()
{
	if (bHasExploded)
	{
		return;
	}

	if (FuseTime <= 0.0f)
	{
		Explode();
		return;
	}

	GetWorldTimerManager().SetTimer(FuseTimerHandle, this, &AGrenadeBase::Explode, FuseTime, false);
}

void AGrenadeBase::ThrowGrenade(const FVector& ThrowDirection)
{
	if (!IsValid(CollisionComponent))
	{
		return;
	}

	const FVector Direction = ThrowDirection.GetSafeNormal();

	if (Direction.IsNearlyZero())
	{
		return;
	}

	CollisionComponent->AddImpulse(Direction * ThrowForce, NAME_None, true);
}

void AGrenadeBase::Explode_Implementation()
{
	if (bHasExploded)
	{
		return;
	}

	bHasExploded = true;

	GetWorldTimerManager().ClearTimer(FuseTimerHandle);

	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), false);

	Destroy();
}