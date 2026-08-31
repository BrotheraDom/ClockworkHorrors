// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "Animation/CharacterAnimationSet.h"
#include "Spells.h"
#include "Staff.generated.h"

class ACharacter;
class ASpellProjectile;
class USceneComponent;
class UStaticMeshComponent;
class UWeaponPickup;

UCLASS(Blueprintable)
class CLOCKWORKHORRORS_API AStaff : public ABaseWeapon
{
    GENERATED_BODY()

public:
    AStaff();


    virtual void BeginPlay() override;

    virtual void Attack() override;

    UFUNCTION(BlueprintCallable, Category = "Staff|Casting")
    void ReleaseSpellCharge();

    UFUNCTION(BlueprintCallable, Category = "Staff|Casting")
    bool AttemptSpellCast(AActor* CastingActor, FVector CastingDirection);

    UFUNCTION(BlueprintPure, Category = "Staff|Casting")
    bool IsSpellReady() const;

    UFUNCTION(BlueprintPure, Category = "Staff|Casting")
    bool IsChargingSpell() const;

    UFUNCTION(BlueprintPure, Category = "Staff|Casting")
    float GetCurrentChargePercent() const;

    UFUNCTION(BlueprintPure, Category = "Staff|Spell")
    ASpells* GetSelectedSpell() const;

    UFUNCTION(BlueprintPure, Category = "Staff|Spell")
    int32 GetSelectedSpellSlot() const;

    UFUNCTION(BlueprintCallable, Category = "Staff|Spell")
    void SelectNextSpell();

    UFUNCTION(BlueprintCallable, Category = "Staff|Spell")
    void SelectPreviousSpell();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Staff|Components")
    TObjectPtr<UStaticMeshComponent> StaffAppearance;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Staff|Components")
    TObjectPtr<USceneComponent> SpellReleasePoint;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Staff|Spell",
        meta = (EditFixedSize)
    )
    TArray<TSubclassOf<ASpells>> SpellSlots;

protected:
    UFUNCTION(BlueprintImplementableEvent, Category = "Staff|Casting")
    void SpellCastRequested(
        AActor* CastingActor,
        FVector CastingDirection,
        ASpells* SelectedSpell
    );

private:
    void SetupStaffInput();
    void UpdateStaffInputConsumption();

    void HandleCastPressed();
    void HandleSpellScrollUp();
    void HandleSpellScrollDown();

    UFUNCTION()
    void HandleStaffWeaponEquippedStateChanged(bool bWeaponEquipped);

    bool IsStaffControlledByPlayer() const;

    bool AttemptSpellCastCharged(AActor* CastingActor, FVector CastingDirection, float ChargePercent);

    UPROPERTY()
    TObjectPtr<UWeaponPickup> WeaponPickup = nullptr;

    UPROPERTY()
    TObjectPtr<ACharacter> AnimationHolder = nullptr;

    TArray<float> SpellCooldownEndTimes;

    bool bStaffInputBindingsCreated = false;
    bool bIsCharging = false;
    float ChargeStartTimeSeconds = 0.0f;
    int32 SelectedSpellIndex = 0;
};