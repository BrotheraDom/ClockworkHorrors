// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "ClockworkPlayerController.h"
#include "BaseCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <WeaponPickup.h>
#include <BaseWeapon.h>
#include <Utils/InventoryComponent.h>
#include "WeaponSlots.h"
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

            EnhancedInputComponent->BindAction(
                ComboAttackAction,
                ETriggerEvent::Completed, this,
                &AClockworkPlayerController::StopAttack
            );

            EnhancedInputComponent->BindAction(
                ComboAttackAction,
                ETriggerEvent::Canceled, this,
                &AClockworkPlayerController::StopAttack
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
        if (WeaponSlotOneAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotOneAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot1
            );
        }
        if (WeaponSlotTwoAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotTwoAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot2
            );
        }
        if (WeaponSlotThreeAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotThreeAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot3
            );
        }
        if (WeaponSlotFourAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotFourAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot4
            );
        }
        if (WeaponSlotFiveAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotFiveAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot5
            );
        }
        if (WeaponSlotSixAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotSixAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot6
            );
        }
        if (WeaponSlotSevenAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotSevenAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot7
            );
        }
        if (WeaponSlotEightAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotEightAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot8
            );
        }
        if (WeaponSlotNineAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotNineAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot9
            );
        }
        if (WeaponSlotTenAction)
        {
            EnhancedInputComponent->BindAction(
                WeaponSlotTenAction,
                ETriggerEvent::Started, this,
                &AClockworkPlayerController::Slot10
            );
        }
        if (DropWeaponAction)
        {
            EnhancedInputComponent->BindAction(
                DropWeaponAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::DropWeapon
            );
        }
        if (ClearInventoryAction)
        {
            EnhancedInputComponent->BindAction(
                ClearInventoryAction,
                ETriggerEvent::Triggered, this,
                &AClockworkPlayerController::ClearInventory
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
    AddYawInput(LookVector.X * LookSensitivity);
    AddPitchInput(LookVector.Y * LookSensitivity);
}

void AClockworkPlayerController::MouseLook(const FInputActionValue& Value)
{
    const FVector2D LookVector = Value.Get<FVector2D>();
    AddYawInput(LookVector.X * MouseSensitivity);
    AddPitchInput(LookVector.Y * MouseSensitivity);
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

void AClockworkPlayerController::StopAttack()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->StopAttack();
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

void AClockworkPlayerController::Slot1()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(1);
    }
}

void AClockworkPlayerController::Slot2()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(2);
    }
}

void AClockworkPlayerController::Slot3()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(3);
    }
}

void AClockworkPlayerController::Slot4()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(4);
    }
}

void AClockworkPlayerController::Slot5()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(5);
    }
}

void AClockworkPlayerController::Slot6()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(6);
    }
}

void AClockworkPlayerController::Slot7()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(7);
    }
}

void AClockworkPlayerController::Slot8()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(8);
    }
}

void AClockworkPlayerController::Slot9()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(9);
    }
}

void AClockworkPlayerController::Slot10()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        PlayerCharacter->EquipWeaponSlot(10);
    }
}

void AClockworkPlayerController::DropWeapon()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        if (PlayerCharacter->weapon)
        {
            UWeaponPickup* basePickup = PlayerCharacter->weapon->GetComponentByClass<UWeaponPickup>();
            PlayerCharacter->DropEquippedWeapon(PlayerCharacter->InventoryComponent->GetItem(basePickup->ItemDataAsset->ItemName));
        }

    }
}

void AClockworkPlayerController::ClearInventory()
{
    if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(GetPawn()))
    {
        //PlayerCharacter->InventoryComponent->ClearInventory();
        for (int i = 0; i < PlayerCharacter->InventoryComponent->GetInventory().Num(); i++)
        {
            PlayerCharacter->DropEquippedWeapon(PlayerCharacter->InventoryComponent->GetInventory()[i]);
        }

    }
}