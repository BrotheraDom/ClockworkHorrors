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

public:
    ACheckpoints();

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

    UFUNCTION(BlueprintCallable, Category = "Checkpoint|Level Travel")
    void LoadNextLevel();

    /*UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    class UBoxComponent* CheckpointCollision;*/

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintCallable, Category = "Checkpoint")
    void SpawnPlayerAtCheckpoint();

    void BoundOverlap(
        UPrimitiveComponent* OverlappedComponent,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

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

    // Enable this only on a checkpoint meant to move the player to another map.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint|Level Travel")
    bool bLoadsNewLevel = false;

    // Example: Level2 for Content/Maps/Level2.umap.
    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Checkpoint|Level Travel",
        meta = (EditCondition = "bLoadsNewLevel")
    )
    FName LevelToLoad;
};