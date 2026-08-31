// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrapPit.generated.h"

class UBoxComponent;

UCLASS()
class CLOCKWORKHORRORS_API ASpikeTrapPit : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	ASpikeTrapPit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnKillVolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Pit")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spike Pit")
	UBoxComponent* KillVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Pit", meta = (ClampMin = "1.0"))
	float KillDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spike Pit")
	bool bTrapEnabled;

};
