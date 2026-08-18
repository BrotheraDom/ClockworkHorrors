// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spawner.generated.h"

USTRUCT(BlueprintType)
struct FWave{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Which index in the 'ActorsToSpawn' array is used when spawning. NOTE: Each Index should be paired with a SpawnCount"))
	TArray<int> ActorToSpawnIndecies;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "How many of the actors should be spawned in this wave. Example Scenario: If ActorToSpawnIndecies[1] is 2 and its pairing ActorToSpawnCount[1] is 5, Whichever actor is index 2 in the ActorsToSpawn array will be spawned 5 times. NOTE: Should always be paired with a ActorToSpawnIndex"))
	TArray<int> ActorToSpawnCounts;
};

UCLASS()
class CLOCKWORKHORRORS_API ASpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool startSpawning;

	UPROPERTY(EditAnywhere)
	float SpawnInterval;
	float Timer;

	/*UPROPERTY(EditAnywhere, meta = (ToolTip = "Max number of actors to spawn. Set to a negative value for unlimited spawns."))
	int32 MaxSpawnCount;*/

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Radius in which actors will spawn. Does nothing if SpawnLocations are set"))
	float SpawnRadius;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "Locations where actors will spawn. If empty, actors will spawn within SpawnRadius of the spawner. SpawnLocations are in WorldSpace, not affected by the position of the spawner"))
	TArray<FVector> SpawnLocations;

	UFUNCTION()
	virtual void BoundOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxCollision;

	UFUNCTION()
	void Spawn();

	UPROPERTY(EditAnywhere, meta = (ToolTip = "If true, after completing the last wave, the spawner will reset back to the first wave"))
	bool bLoopWaves;

	bool bWaveInProgress;

	bool finishedSpawning;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "All actors that this spawner can spawn"))
	TArray<TSubclassOf<AActor>> ActorsToSpawn;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "If true, subsequent waves will only spawn once all current enemies from the spawner are destroyed"))
	bool bWaveSpawnAfterClear;

	int32 SpawnedActorCount;

	UPROPERTY(EditAnywhere, meta = (ToolTip = "All waves that this spawner will spawn"))
	TArray<FWave> Waves;

	int CurrentWaveIndex;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* Mesh;

	UFUNCTION()
	void OnSpawnedActorDestroyed(AActor* DestroyedActor);

};
