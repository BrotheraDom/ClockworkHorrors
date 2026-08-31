// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"

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

void AProjectile::BeginPlay()
{
    Super::BeginPlay();

    ProjectileMesh->SetWorldScale3D(Size);
    SphereCollision->SetWorldScale3D(FVector(0.18f, 0.18f, 0.18f));

    FTimerHandle DestroyTimer;

    GetWorld()->GetTimerManager().SetTimer(
        DestroyTimer,
        this,
        &AProjectile::K2_DestroyActor,
        DestroyTime
    );

    controller = GetWorld()->GetFirstPlayerController();

    if (!controller)
    {
        Destroy();
        return;
    }

    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = MaxSpeed;
    ProjectileMovementComponent->ProjectileGravityScale = ProjectileGravityScale;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;

   
    ProjectileMovementComponent->Velocity =
        GetActorForwardVector() * InitialSpeed;
}

void AProjectile::HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);
	if (Cast<AEnemy>(OtherActor))
	{
		OtherActor->TakeDamage(damage, DamageEvent, controller, this);
		Destroy();
	}

}

