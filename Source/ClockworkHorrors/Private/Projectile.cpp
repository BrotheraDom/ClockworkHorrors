// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SphereCollision = CreateDefaultSubobject<USphereComponent>("SphereCollision");
	// Like dragging the component to the root
	SetRootComponent(SphereCollision);
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("SphereMesh");
	ProjectileMesh->SetCollisionProfileName("NoCollision");
	ProjectileMesh->SetupAttachment(GetRootComponent());
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::HandleOverlap);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = ProjectileGravityScale;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->SetWorldScale3D(Size);
	SphereCollision->SetWorldScale3D(FVector(0.18, 0.18, 0.18));

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &AProjectile::K2_DestroyActor, DestroyTime);

	controller = GetWorld()->GetFirstPlayerController();
	if (!controller)
	{
		Destroy();
	}

	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
	ProjectileMovementComponent->MaxSpeed = MaxSpeed;
	ProjectileMovementComponent->ProjectileGravityScale = ProjectileGravityScale;
	ProjectileMovementComponent->Velocity = GetActorForwardVector() * InitialSpeed;
}

void AProjectile::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);
	if (OtherActor->GetInstigatorController() != controller)
	{
		OtherActor->TakeDamage(damage, DamageEvent, controller, this);
	}
	Destroy();
}

