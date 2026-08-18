// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Spawner.h"
#include "Components/BoxComponent.h"

// Sets default values
ASpawner::ASpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
	BoxCollision->SetupAttachment(Mesh);

}

// Called when the game starts or when spawned
void ASpawner::BeginPlay()
{
	Super::BeginPlay();
	startSpawning = false;
	finishedSpawning = false;
	Timer = 0;
	CurrentWaveIndex = 0;
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpawner::BoundOverlap);
	
}

// Called every frame
void ASpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timer += DeltaTime;
	if (!finishedSpawning && Timer >= SpawnInterval && startSpawning /*&& (MaxSpawnCount > 0 || MaxSpawnCount < 0)*/)
	{
		if (!bWaveSpawnAfterClear || (bWaveSpawnAfterClear && SpawnedActorCount <= 0))
		{
			Spawn();

		}
		Timer = 0;
		/*if (MaxSpawnCount > 0)
		{
			MaxSpawnCount--;
		}*/
	}

}

void ASpawner::BoundOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	startSpawning = true;
}

void ASpawner::Spawn()
{
	if (Waves.Num() > 0)
	{
		FVector SpawnLocation;
		for (int32 i = 0; i < Waves[CurrentWaveIndex].ActorToSpawnIndecies.Num(); i++)
		{
			for (int32 j = 0; j < Waves[CurrentWaveIndex].ActorToSpawnCounts[i]; j++)
			{
				int32 ActorIndex = Waves[CurrentWaveIndex].ActorToSpawnIndecies[i];

				if (!ActorsToSpawn.IsValidIndex(ActorIndex))
				{
					UE_LOG(LogTemp, Error, TEXT("ActorToSpawnIndecies contains invalid index %d. ActorsToSpawn array has %d elements."),
						ActorIndex, ActorsToSpawn.Num());
					Destroy();
				}

				if (!ActorsToSpawn[ActorIndex])
				{
					UE_LOG(LogTemp, Error, TEXT("ActorToSpawnIndecies contains index %d which points to a null class in ActorsToSpawn array."), ActorIndex);
					Destroy();
				}

				if (SpawnLocations.Num() > 0)
				{
					int32 RandomIndex = FMath::RandRange(0, SpawnLocations.Num() - 1);
					SpawnLocation = SpawnLocations[RandomIndex];
				}
				else
				{
					SpawnLocation = GetActorLocation();
					SpawnLocation.X += FMath::RandRange(-SpawnRadius, SpawnRadius);
					SpawnLocation.Y += FMath::RandRange(-SpawnRadius, SpawnRadius);
				}
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = this;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorsToSpawn[ActorIndex], SpawnLocation, GetActorRotation(), SpawnParams);

				if (!SpawnedActor)
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to spawn actor of class %s"), *ActorsToSpawn[ActorIndex]->GetName());
				}
				else {
					SpawnedActorCount++;
					SpawnedActor->OnDestroyed.AddDynamic(this, &ASpawner::OnSpawnedActorDestroyed);
				}
			}
		}
		if (bLoopWaves)
		{
			CurrentWaveIndex = (CurrentWaveIndex + 1) % Waves.Num();
		}
		else
		{
			CurrentWaveIndex++;
			if (CurrentWaveIndex >= Waves.Num())
			{
				finishedSpawning = true;
			}
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ActorToSpawn is not set in the Spawner"));
	}
}

void ASpawner::OnSpawnedActorDestroyed(AActor* DestroyedActor)
{
	SpawnedActorCount--;
}

