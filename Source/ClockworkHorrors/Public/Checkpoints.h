// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spawner.h"
#include "GameFramework/Actor.h"
#include "Checkpoints.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API ACheckpoints : public ASpawner
{
	GENERATED_BODY()

	ACheckpoints();

public:
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void ActivateCheckpoint();

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void CheckpointReached();

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void DeactivateCheckpoint();

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool IsCheckpointActive() const { return bIsActive; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetCheckpointActive(bool bActive) { bIsActive = bActive; }

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	bool IsFinalCheckpoint() const { return bIsFinalCheckpoint; }

	/*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UBoxComponent* CheckpointCollision;*/

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SpawnPlayerAtCheckpoint();

	void BoundOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	class UStaticMeshComponent* CheckpointMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool bIsActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	bool bIsFinalCheckpoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	AActor* PlayerActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	TSubclassOf<AActor> PlayerActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint")
	class ACheckpointManager* CheckpointManager;
	
};
