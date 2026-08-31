// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/MovingWallTrap.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

// Sets default values
AMovingWallTrap::AMovingWallTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WallCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("WallCollision"));
	RootComponent = WallCollision;

	WallCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WallCollision->SetCollisionObjectType(ECC_WorldDynamic);

	WallCollision->SetCollisionResponseToAllChannels(ECR_Block);
	WallCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	WallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WallMesh"));
	WallMesh->SetupAttachment(WallCollision);

	WallMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

// Called when the game starts or when spawned
void AMovingWallTrap::BeginPlay()
{
	Super::BeginPlay();
	
	StartLocation = GetActorLocation();
	EndLocation = StartLocation + MoveOffset;
}

// Called every frame
void AMovingWallTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWaiting)
	{
		CurrentMovementDelta = FVector::ZeroVector;
		return;
	}

	MoveWall(DeltaTime);
}

void AMovingWallTrap::MoveWall(float DeltaTime)
{
	const FVector CurrentLocation = GetActorLocation();

	const FVector Destination = bMovingForward ? EndLocation : StartLocation;

	const FVector ToDestination = Destination - CurrentLocation;
	const float DistanceRemaining = ToDestination.Size();

	if (DistanceRemaining <= KINDA_SMALL_NUMBER)
	{
		SetActorLocation(Destination);

		bMovingForward = !bMovingForward;

		StartWait();

		return;
	}

	const FVector MovementDirection = ToDestination.GetSafeNormal();

	const float MovementDistance = MoveSpeed * DeltaTime;

	FVector NewLocation;

	if (MovementDistance >= DistanceRemaining)
	{
		NewLocation = Destination;
	}
	else
	{
		NewLocation = CurrentLocation + (MovementDirection * MovementDistance);
	}

	CurrentMovementDelta = NewLocation - CurrentLocation;

	FHitResult HitResult;

	SetActorLocation(NewLocation,true, &HitResult, ETeleportType::None);

	if (HitResult.bBlockingHit)
	{
		if (ACharacter* Character = Cast<ACharacter>(HitResult.GetActor()))
		{
			PushPlayer(Character);

			if (bDealsDamage && DamageAmount > 0.0f)
			{
				UGameplayStatics::ApplyDamage(Character, DamageAmount, GetInstigatorController(), this, UDamageType::StaticClass());
			}

			FHitResult SecondHit;

			SetActorLocation(NewLocation, true, &SecondHit, ETeleportType::None);
		}
	}

	const FVector UpdatedLocation = GetActorLocation();

	if (FVector::DistSquared(UpdatedLocation, Destination) <= FMath::Square(1.0f))
	{
		SetActorLocation(Destination);

		bMovingForward = !bMovingForward;

		StartWait();
	}
}

void AMovingWallTrap::StartWait()
{
	if (ReturnDelay <= 0.0f)
	{
		bWaiting = false;
		return;
	}

	bWaiting = true;

	GetWorldTimerManager().SetTimer(WaitTimerHandle, [this]() { bWaiting = false; }, ReturnDelay, false);
}

void AMovingWallTrap::PushPlayer(ACharacter* Character)
{
	if (!Character || CurrentMovementDelta.IsNearlyZero())
	{
		return;
	}

	UCharacterMovementComponent* CharacterMovement = Character->GetCharacterMovement();

	if (!CharacterMovement || !CharacterMovement->UpdatedComponent)
	{
		return;
	}

	FHitResult PushHit;

	CharacterMovement->SafeMoveUpdatedComponent(CurrentMovementDelta, CharacterMovement->UpdatedComponent->GetComponentQuat(), 
		true,PushHit
	);
}