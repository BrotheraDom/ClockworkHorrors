// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellLingeringField.generated.h"

class USceneComponent;
class USphereComponent;
class UBoxComponent;
class UPrimitiveComponent;
class ASpells;

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API ASpellLingeringField : public AActor
{
	GENERATED_BODY()

public:
	ASpellLingeringField();

	void InitializeLingeringField(AActor* InCastingActor, ASpells* InSpellDefinition);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lingering Field")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lingering Field")
	USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lingering Field")
	UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lingering Field|Debug")
	bool bShowLingeringHitbox;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AActor* CastingActor;

	UPROPERTY()
	ASpells* SpellDefinition;

	FTimerHandle PulseTimer;

	void ApplyLingeringPulse();

	UPrimitiveComponent* GetActiveFieldCollision() const;
};