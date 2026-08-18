// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckpointManager.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API ACheckpointManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckpointManager();

	UFUNCTION(BlueprintCallable, Category = "Checkpoints")
	void ChangeCurrentCheckpoint(ACheckpoints* NewCheckpoint);

	UFUNCTION(BlueprintCallable, Category = "Checkpoints")
	void RespawnPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Checkpoints")
	void RespawnPlayerAtStart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	TArray<class ACheckpoints*> OldCheckpointsArray;

	// pointer to the PlayerStart object in the level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoints")
	TSoftObjectPtr<class APlayerStart> PlayerStart;

private:
	class ACheckpoints* CurrentActiveCheckpoint;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
