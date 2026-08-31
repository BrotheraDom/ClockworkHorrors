// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/CompanionInterface.h"
#include "Components/SpellManager.h"
#include "BaseCompanion.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API ABaseCompanion : public ACharacter, public ICompanionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCompanion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UPROPERTY(VisibleAnywhere)
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere)
	class UManaComponent* ManaComponent;

	UPROPERTY(VisibleAnywhere)
	class USpellManager* SpellManager;

	UFUNCTION()
	void HandleActionFinished();

	UFUNCTION()
	void HandleDeath();

	UPROPERTY(EditDefaultsOnly)
	FName ActionFinishedMessage;

	FTimerHandle AttackResetTimerHandle;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Companion|AI"
	)
	float MoveSpeed = 250.0f;

public:
	virtual bool CastSpellSlot(int spellslot, AActor* Target) override;
	virtual void UpdateTarget(AActor* NewTarget, float damage) override;

};
