// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrenadeBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class CLOCKWORKHORRORS_API AGrenadeBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGrenadeBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grenade|Components")
	UStaticMeshComponent* GrenadeMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Throw")
	float ThrowForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Explosion")
	float FuseTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Explosion")
	float ExplosionDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Explosion")
	float ExplosionRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grenade|Explosion")
	bool bUseFuseTimer;

	UPROPERTY(BlueprintReadOnly, Category = "Grenade|State")
	bool bHasExploded;

	FTimerHandle FuseTimerHandle;

	UFUNCTION(BlueprintCallable, Category = "Grenade")
	virtual void StartFuse();

public:	

	UFUNCTION(BlueprintCallable, Category = "Grenade")
	virtual void ThrowGrenade(const FVector& ThrowDirection);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Grenade")
	void Explode();

	virtual void Explode_Implementation();

	UFUNCTION(BlueprintPure, Category = "Grenade")
	bool HasExploded() const { return bHasExploded; }
};
