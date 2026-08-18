// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/DamageAreaBase.h"
#include "SpikeTrap.generated.h"

class UBoxComponent;
class UInstancedStaticMeshComponent;
class ABaseCharacter;

UENUM(BlueprintType)
enum class ESpikeTrapState : uint8
{
	Ready,
	Waiting,
	Rising,
	Extended,
	Retracting,
	Cooldown
};

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API ASpikeTrap : public ADamageAreaBase
{
	GENERATED_BODY()
	
public:
	ASpikeTrap();

	virtual void Tick(float DeltaTime) override;

	virtual void OnConstruction(
		const FTransform& Transform
	) override;

protected:
	virtual void BeginPlay() override;

	// Holds all spike instances.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap")
	TObjectPtr<UInstancedStaticMeshComponent> SpikeMeshes;

	// Detects the player and activates the trap.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap")
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Trap")
	TObjectPtr<UBoxComponent> SpikeDamageBox;

	// Number of spikes across X.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Layout", meta = (ClampMin = "1") )
	int32 SpikesX = 1;

	// Number of spikes across Y.
	// Keep at 1 for a row.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Layout", meta = (ClampMin = "1"))
	int32 SpikesY = 1;

	// Distance between spikes.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Layout", meta = (ClampMin = "1.0"))
	float SpikeSpacing = 100.0f;

	// Delay before spikes rise.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Timing", meta = (ClampMin = "0.0"))
	float ActivationDelay = 0.25f;

	// Distance the spikes travel underground.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Movement", meta = (ClampMin = "0.0"))
	float SpikeTravelDistance = 100.0f;

	// Speed spikes rise.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Movement", meta = (ClampMin = "1.0"))
	float RiseSpeed = 500.0f;

	// Speed spikes retract.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Movement", meta = (ClampMin = "1.0"))
	float RetractSpeed = 300.0f;

	// Time spikes stay above ground.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Timing", meta = (ClampMin = "0.0"))
	float ExtendedTime = 1.0f;

	// Delay before another activation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Trap|Timing", meta = (ClampMin = "0.0"))
	float CooldownTime = 2.0f;

	UFUNCTION()
	void OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	void OnSpikeDamageBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void BuildSpikeLayout();
	void UpdateCollisionSize();
	void QueueActivation();
	void StartRising();
	void StartRetracting();
	void StartCooldown();
	void ResetTrap();

private:
	ESpikeTrapState TrapState = ESpikeTrapState::Ready;

	float CurrentSpikeOffset = 0.0f;

	UPROPERTY()
	TObjectPtr<ABaseCharacter> PlayerInTrigger = nullptr;

	FTimerHandle ActivationTimerHandle;
	FTimerHandle ExtendedTimerHandle;
	FTimerHandle CooldownTimerHandle;
};
