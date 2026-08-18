// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "ClockworkPlayerController.h"
#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void AClockworkPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
    {
        if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
            LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
        {
            if (DefaultMappingContext)
            {
                InputSubsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void AClockworkPlayerController::SetupInputComponent()
{
    
    APlayerController::SetupInputComponent();

    if (!InputComponent)
    {
        return;
    }

    if (UEnhancedInputComponent* EnhancedInputComponent =
        Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(
                MoveAction,
                ETriggerEvent::Triggered,
                this,
                &AClockworkPlayerController::Move
            );
        }

        if (LookAction)
        {
            EnhancedInputComponent->BindAction(
                LookAction,
                ETriggerEvent::Triggered,
                this,
                &AClockworkPlayerController::Look
            );
        }

        if (MouseLookAction)
        {
            EnhancedInputComponent->BindAction(
                MouseLookAction,
                ETriggerEvent::Triggered,
                this,
                &AClockworkPlayerController::MouseLook
            );
        }

        if (JumpAction)
        {
            EnhancedInputComponent->BindAction(
                JumpAction,
                ETriggerEvent::Started,
                this,
                &AClockworkPlayerController::StartJump
            );

            EnhancedInputComponent->BindAction(
                JumpAction,
                ETriggerEvent::Completed,
                this,
                &AClockworkPlayerController::StopJump
            );
        }
        if (InteractAction)
        {
            EnhancedInputComponent->BindAction(
                InteractAction,
                ETriggerEvent::Triggered,
                this,
                &AClockworkPlayerController::Interact
                );
        }
        if (ComboAttackAction)
        {
            EnhancedInputComponent->BindAction(
                ComboAttackAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::Attack
            );
        }
        if (ReloadAction)
        {
            EnhancedInputComponent->BindAction(
                ReloadAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::Reload
            );
        }
        if (PauseAction)
        {
            EnhancedInputComponent->BindAction(
                PauseAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::HandlePauseInput
            );
        }
        if (QuitGameAction)
        {
            EnhancedInputComponent->BindAction(
                QuitGameAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::HandleQuitInput
            );
        }
        if (InventoryAction)
        {
            EnhancedInputComponent->BindAction(
                InventoryAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::OpenInventory
            );
        }
        if (KillAction)
        {
            EnhancedInputComponent->BindAction(
                KillAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::KillPlayer
            );
        }
    }


 
}

void AClockworkPlayerController::Move(const FInputActionValue& Value)
{
    if (IsMoveInputIgnored())
    {
        return;
    }

    ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn());
    if (!ControlledCharacter)
    {
        return;
    }

    const FVector2D MovementVector = Value.Get<FVector2D>();

    const FRotator PlayerControlRotation = GetControlRotation();
    const FRotator YawRotation(0.0f, PlayerControlRotation.Yaw, 0.0f);

    const FVector ForwardDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

    const FVector RightDirection =
        FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    ControlledCharacter->AddMovementInput(
        ForwardDirection,
        MovementVector.Y
    );

    ControlledCharacter->AddMovementInput(
        RightDirection,
        MovementVector.X
    );
}

void AClockworkPlayerController::Look(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();

    AddYawInput(LookVector.X);
    AddPitchInput(LookVector.Y);
}

void AClockworkPlayerController::MouseLook(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();

    AddYawInput(LookVector.X);
    AddPitchInput(LookVector.Y);
}

void AClockworkPlayerController::StartJump()
{
    if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
    {
        ControlledCharacter->Jump();
    }
}

void AClockworkPlayerController::StopJump()
{
    if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
    {
        ControlledCharacter->StopJumping();
    }
}

void AClockworkPlayerController::Interact()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
		PlayerCharacter->TryPickupInteract();
    }
}

void AClockworkPlayerController::HandlePauseInput()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->HandlePausePressed();
    }
}

void AClockworkPlayerController::HandleQuitInput()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->QuitGameFromPause();
    }
}
void AClockworkPlayerController::Attack()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->Attack();
    }
}
void AClockworkPlayerController::Reload()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->Reload();
    }
}

void AClockworkPlayerController::OpenInventory()
{
    UE_LOG(LogTemp, Warning, TEXT("Called Open Inventory"));
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->InventoryHUD();
    }
}

void AClockworkPlayerController::KillPlayer()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->HandleDeath();
    }
}
