// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Spells.h"
#include "Staff.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UWeaponPickup;
class ASpellProjectile;

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API AStaff : public AActor
{
	GENERATED_BODY()

public:
	AStaff();

	UFUNCTION(BlueprintCallable, Category = "Staff|Casting")
	bool AttemptSpellCast(AActor* CastingActor, FVector CastingDirection);

	UFUNCTION(BlueprintPure, Category = "Staff|Casting")
	bool IsSpellReady() const;

	UFUNCTION(BlueprintPure, Category = "Staff|Spell")
	ASpells* GetSelectedSpell() const;

	UFUNCTION(BlueprintPure, Category = "Staff|Spell")
	int32 GetSelectedSpellSlot() const;

	UFUNCTION(BlueprintCallable, Category = "Staff|Spell")
	void SelectNextSpell();

	UFUNCTION(BlueprintCallable, Category = "Staff|Spell")
	void SelectPreviousSpell();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Staff|Components")
	UStaticMeshComponent* StaffAppearance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Staff|Components")
	USceneComponent* SpellReleasePoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Staff|Spell", meta = (EditFixedSize))
	TArray<TSubclassOf<ASpells>> SpellSlots;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Staff|Casting")
	void SpellCastRequested(AActor* CastingActor, FVector CastingDirection, ASpells* SelectedSpell);

private:
	UPROPERTY()
	UWeaponPickup* WeaponPickup;

	bool bSpellReady;
	bool bStaffInputBindingsCreated;

	int32 SelectedSpellIndex;

	FTimerHandle SpellRecoveryTimer;

	void SetupStaffInput();

	void HandleCastPressed();
	void HandleSpellScrollUp();
	void HandleSpellScrollDown();

	UFUNCTION()
	void HandleWeaponEquippedStateChanged(bool bWeaponEquipped);

	void UpdateStaffInputConsumption();

	bool IsStaffControlledByPlayer() const;

	void FinishSpellRecovery();
};