// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	AController* controller;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class USphereComponent* SphereCollision;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector Size;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float DestroyTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float damage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int InitialSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int MaxSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int ProjectileGravityScale;
public:	
	UFUNCTION()
	void HandleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
