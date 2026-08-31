#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "CompanionSpawner.generated.h"

class USoundBase;
class UNiagaraComponent;
class ACompanionCharacter; // ?? Forward declare CompanionCharacter

UCLASS()
class CLOCKWORKHORRORS_API ACompanionSpawner : public AActor
{
	GENERATED_BODY()

public:
	ACompanionSpawner();

	/** Triggers portal VFX/SFX, spawns companion behind it, and returns the companion reference. */
	UFUNCTION(BlueprintCallable, Category = "Companion Spawn")
	ACompanionCharacter* TriggerSpawnSequence();

	/** Getter for the companion instance created by this spawner. */
	UFUNCTION(BlueprintPure, Category = "Companion Spawn")
	ACompanionCharacter* GetSpawnedCompanion() const { return SpawnedCompanion; }

protected:
	/** Companion class to instantiate (select BP_CompanionCharacter in Details) */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings")
	TSubclassOf<ACompanionCharacter> CompanionClass;

	/** Portal Niagara VFX System */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings|VFX")
	TObjectPtr<UNiagaraSystem> PortalVFX;

	/** Portal opening sound effect */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings|Audio")
	TObjectPtr<USoundBase> PortalSound;

	/** Distance behind the portal where the companion physically spawns */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings|Offset")
	float SpawnDistanceBehindPortal = 150.0f;

	/** Distance in front of the portal the companion walks toward */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings|Offset")
	float WalkOutDistance = 250.0f;

	/** Delay between portal opening and companion walking out */
	UPROPERTY(EditAnywhere, Category = "Spawn Settings|Timing")
	float DelayBeforeWalkOut = 0.5f;

private:
	void Internal_SpawnAndWalk();

	UPROPERTY()
	TObjectPtr<ACompanionCharacter> SpawnedCompanion;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedPortalComponent;

	FTimerHandle SpawnTimerHandle;
};