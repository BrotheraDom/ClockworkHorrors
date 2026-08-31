// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Testing/TargetLockTestPlayerController.h"

#include "Targeting/TargetLockComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

void ATargetLockTestPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (TargetLockMappingContext)
            {
                // Higher priority than the normal mapping context, but it only
                // contains the experimental lock-on actions.
                InputSubsystem->AddMappingContext(TargetLockMappingContext, 10);
            }
        }
    }
}

void ATargetLockTestPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // LocalPlayer survives some level changes, so explicitly remove the test
    // context before leaving the test map.
    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (TargetLockMappingContext)
            {
                InputSubsystem->RemoveMappingContext(TargetLockMappingContext);
            }
        }
    }

    Super::EndPlay(EndPlayReason);
}

void ATargetLockTestPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent);
    if (!EnhancedInput)
    {
        return;
    }

    if (TargetLockAction)
    {
        EnhancedInput->BindAction(
            TargetLockAction,
            ETriggerEvent::Started,
            this,
            &ATargetLockTestPlayerController::ToggleTargetLock
        );
    }

    if (TargetSwitchAction)
    {
        // Started is deliberate: a held analog stick should switch once when
        // it crosses the action threshold, not every frame.
        EnhancedInput->BindAction(
            TargetSwitchAction,
            ETriggerEvent::Started,
            this,
            &ATargetLockTestPlayerController::HandleTargetSwitch
        );
    }

    if (LookAction)
    {
        // AClockworkPlayerController is still bound to this action for normal
        // unlocked camera look. While locked, TargetLockComponent calls
        // SetIgnoreLookInput(true), so the base AddYaw/AddPitch path is
        // suppressed while this extra binding receives the same right-stick
        // value for the bounded combat-camera orbit.
        EnhancedInput->BindAction(
            LookAction,
            ETriggerEvent::Triggered,
            this,
            &ATargetLockTestPlayerController::HandleLockedCameraLook
        );
    }

    if (MouseLookAction)
    {
        // Same idea as the right stick, but mouse input is a per-frame delta
        // and therefore uses a separate sensitivity path in TargetLockComponent.
        EnhancedInput->BindAction(
            MouseLookAction,
            ETriggerEvent::Triggered,
            this,
            &ATargetLockTestPlayerController::HandleLockedCameraMouseLook
        );
    }
}

void ATargetLockTestPlayerController::ToggleTargetLock()
{
    if (UTargetLockComponent* TargetLock = GetTargetLockComponent())
    {
        TargetLock->ToggleTargetLock();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ZLock Test: possessed pawn has no TargetLockComponent."));
    }
}

void ATargetLockTestPlayerController::HandleTargetSwitch(const FInputActionValue& Value)
{
    UTargetLockComponent* TargetLock = GetTargetLockComponent();
    if (!TargetLock || !TargetLock->IsTargetLocked())
    {
        return;
    }

    const float Direction = Value.Get<float>();

    if (Direction < -KINDA_SMALL_NUMBER)
    {
        TargetLock->SwitchTargetLeft();
    }
    else if (Direction > KINDA_SMALL_NUMBER)
    {
        TargetLock->SwitchTargetRight();
    }
}

void ATargetLockTestPlayerController::HandleLockedCameraLook(const FInputActionValue& Value)
{
    UTargetLockComponent* TargetLock = GetTargetLockComponent();
    if (!TargetLock || !TargetLock->IsTargetLocked())
    {
        return;
    }

    TargetLock->AddLockedCameraLookInput(Value.Get<FVector2D>());
}

void ATargetLockTestPlayerController::HandleLockedCameraMouseLook(const FInputActionValue& Value)
{
    UTargetLockComponent* TargetLock = GetTargetLockComponent();
    if (!TargetLock || !TargetLock->IsTargetLocked())
    {
        return;
    }

    TargetLock->AddLockedCameraMouseLookInput(Value.Get<FVector2D>());
}

UTargetLockComponent* ATargetLockTestPlayerController::GetTargetLockComponent() const
{
    const APawn* ControlledPawn = GetPawn();
    return ControlledPawn
        ? ControlledPawn->FindComponentByClass<UTargetLockComponent>()
        : nullptr;
}
