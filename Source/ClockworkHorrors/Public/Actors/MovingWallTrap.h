// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MovingWallTrap.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class ACharacter;

UCLASS()
class CLOCKWORKHORRORS_API AMovingWallTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovingWallTrap();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Wall")
	UBoxComponent* WallCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving Wall")
	UStaticMeshComponent* WallMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Wall|Movement")
	FVector MoveOffset = FVector(500.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Wall|Movement", meta = (ClampMin = "1.0"))
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Moving Wall|Movement", meta = (ClampMin = "0.0"))
	float ReturnDelay = 1.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	bool bDealsDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (EditCondition = "bDealsDamage", AllowPrivateAccess = "true"))
	float DamageAmount = 25.0f;

	FVector StartLocation;
	FVector EndLocation;
	FVector CurrentMovementDelta;

	bool bMovingForward = true;
	bool bWaiting = false;

	FTimerHandle WaitTimerHandle;

	void MoveWall(float DeltaTime);
	void StartWait();
	void PushPlayer(ACharacter* Character);
};
