// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ClockworkPlayerController.h"
#include "InputActionValue.h"
#include "TargetLockTestPlayerController.generated.h"

class UInputAction;
class UInputMappingContext;
class UTargetLockComponent;

/**
 * Test-only controller for the Z-lock prototype.
 *
 * Use this only in BP_ZLockTestGameMode. The normal project controller remains
 * unchanged. The test mapping context is removed again when this controller
 * leaves the world so it cannot leak into normal maps.
 */
UCLASS()
class CLOCKWORKHORRORS_API ATargetLockTestPlayerController : public AClockworkPlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void SetupInputComponent() override;

private:
    void ToggleTargetLock();
    void HandleTargetSwitch(const FInputActionValue& Value);
    void HandleLockedCameraLook(const FInputActionValue& Value);
    void HandleLockedCameraMouseLook(const FInputActionValue& Value);
    UTargetLockComponent* GetTargetLockComponent() const;

public:
    /** Mapping context containing only prototype lock/switch bindings. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Target Lock")
    UInputMappingContext* TargetLockMappingContext;

    /** Digital action. Suggested bindings: Q and Gamepad Right Thumbstick (R3). */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Target Lock")
    UInputAction* TargetLockAction;

    /** Axis1D action. Negative = left, positive = right. */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Target Lock")
    UInputAction* TargetSwitchAction;
};
