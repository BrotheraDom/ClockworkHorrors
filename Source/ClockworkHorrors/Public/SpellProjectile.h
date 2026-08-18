// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class ASpells;

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API ASpellProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASpellProjectile();

	void InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* ProjectileAppearance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

private:
	UPROPERTY()
	AActor* CastingActor;

	UPROPERTY()
	ASpells* SpellDefinition;
};