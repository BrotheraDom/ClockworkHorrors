// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellProjectile.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;
class UPrimitiveComponent;
class ASpells;

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API ASpellProjectile : public AActor
{
	GENERATED_BODY()

public:
	ASpellProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection);

	void InitializeProjectile(AActor* InCastingActor, ASpells* InSpellDefinition, const FVector& InTravelDirection, float InChargePercent);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* ProjectileAppearance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Debug")
	bool bShowProjectileHitbox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Debug")
	bool bShowLingeringHitbox;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void ProjectileImpact(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void ProjectileOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	UPROPERTY()
	AActor* CastingActor;

	UPROPERTY()
	ASpells* SpellDefinition;

	UPROPERTY()
	TArray<AActor*> DamagedActors;

	int32 CurrentBounceCount;
	int32 CurrentPierceCount;

	float CurrentImpactDamage;
	float ChargePercent;

	void CreateLingeringField(const FVector& FieldLocation);

	bool ApplyImpactDamageOnce(AActor* OtherActor);

	void AcquireHomingTarget();
};