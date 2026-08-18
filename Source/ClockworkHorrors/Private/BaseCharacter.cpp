// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "BaseCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Misc/Optional.h"
#include "BasePickup.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Actors/LavaDamageArea.h"
#include "Actors/DamageAreaBase.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Colors/SColorBlock.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Styling/CoreStyle.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Utils/HealthComponent.h"
#include "Utils/InventoryComponent.h"

#include "CheckpointManager.h"
#include "BaseBlaster.h"
#include "UI/PauseMenuWidget.h"
#include "UI/GameOverWidget.h"
#include "UI/OptionsMenuWidget.h"
#include "UI/MainInventoryWidget.h"
#include "Components/CharacterAnimationComponent.h"
#include <WeaponPickup.h>

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	//CurrentHealth = MaxHealth;
	/*RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));

	RifleMesh->SetupAttachment(GetMesh(), TEXT("Weapon_r"));


	RifleMesh->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	RifleMesh->SetRelativeRotation(FRotator(90.0f, 0.0f, -90.0f));

	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RifleMesh->SetGenerateOverlapEvents(false);
	RifleMesh->SetVisibility(false);*/

	// Components
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

}


void ABaseCharacter::TryPickupInteract()
{
	//UE_LOG(LogTemp, Warning, TEXT("TryPickupInteract called"));
	TArray<AActor*> OverlappingActors;



	///REMOVE THIS BLOCK OF CODE LATER, THIS IS JUST FOR TESTING INVENTORTY COMPONENT FUNCTIONALITY

	GetOverlappingActors(OverlappingActors, ABasePickup::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		ABasePickup* BasePickup = Cast<ABasePickup>(Actor);

		if (!BasePickup)
		{
			continue;
		}
		BasePickup->OnInteract();
		return;
	}








	GetOverlappingActors(OverlappingActors, UWeaponPickup::StaticClass());

	for (AActor* Actor : OverlappingActors)
	{
		ABaseWeapon* baseWeapon = Cast<ABaseWeapon>(Actor);
		if (baseWeapon)
		{
			if (!baseWeapon->bPickedUp)
			{
				UWeaponPickup* BasePickup = baseWeapon->FindComponentByClass<UWeaponPickup>();
				if (!BasePickup)
				{
					continue;
				}
				//BasePickup->OnWeaponEquippedStateChanged.Broadcast();
				//if (weapon)
				//{
				//	if (BasePickup->GetInventorySlot() == weapon->FindComponentByClass<UWeaponPickup>()->GetInventorySlot())
				//	{
				//		continue;
				//	}
				//}
				BasePickup->HandleInteractPressed();
				InventoryComponent->AddItem(BasePickup->ItemDataAsset);
				{
					if (BasePickup->ItemDataAsset->WeaponClass)
					{
						weaponClass = BasePickup->ItemDataAsset->WeaponClass;
						weapon = Cast<ABaseWeapon>(Actor);
						EquipPickupWeapon();
					}


				}



			}

		}
		return;
	}

}
void ABaseCharacter::EquipPickupWeapon()
{
	// THIS NEEDS TO SPAWN THE WEAPON FROM THE PICKUP'S ITEM DATA ASSET
	bHasWeapon = true;
	if (weapon)
	{
		weapon->ParentPawn = Cast<APawn>(this);
		weapon->AttachToComponent(
			GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale,
			weapon->WeaponSocket
		);
		weapon->bPickedUp = true;
	}

}
//
//float ABaseCharacter::ApplyDamage(float DamageAmount)
//{
//	return 0.0f;
//}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsDead = false;
	if (HealthComponent)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Current Health: %f"), HealthComponent->GetCurrentHealth());
		HealthComponent->OnCharacterDeath.AddDynamic(
			this,
			&ABaseCharacter::HandleDeath
		);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent is missing on %s"), *GetName());
	}
	// Find the reusable animation component attached to this character.
	CharacterAnimationComponent = FindComponentByClass<UCharacterAnimationComponent>();

	if (CharacterAnimationComponent)
	{
		CharacterAnimationComponent->OnActionAnimationEnded.AddDynamic(
			this,
			&ABaseCharacter::HandleActionAnimationEnded
		);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("CharacterAnimationComponent is missing on %s"),
			*GetName()
		);
	}

	// Keep the existing attack delegate so any current weapon/gameplay code
	// that broadcasts OnAttack continues to work.
	OnAttack.AddDynamic(this, &ABaseCharacter::AttackingAnim);
	bGameOverOverlayShown = false;
	GameOverSlateWidget.Reset();

	bPauseOverlayShown = false;
	PauseSlateWidget.Reset();

	bControlsOverlayShown = false;
	ControlsSlateWidget.Reset();

	bStartScreenShown = false;
	StartSlateWidget.Reset();

	bHealthBarShown = false;
	HealthBarSlateWidget.Reset();

	ShowStartScreen();


	//Find the CheckpointManager in the world
	CheckpointManager = Cast<ACheckpointManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACheckpointManager::StaticClass()));
	if (!CheckpointManager)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckpointManager not found in the world!"));
	}

	if (InventoryComponent)
	{
		InventoryComponent->OnWeaponPickedUp.AddDynamic(this, &ABaseCharacter::EquipPickupWeapon);
	}


	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UUserWidget* InventoryHud = CreateWidget(GetWorld()->GetFirstPlayerController(), InventoryHudClass);
		InventoryWidget = Cast<UMainInventoryWidget>(InventoryHud);

		if (InventoryWidget)
		{
			InventoryWidget->AddToViewport();
			InventoryWidget->SetVisibility(ESlateVisibility::Hidden);

			if (InventoryComponent)
			{
				InventoryComponent->OnItemDataAdded.AddDynamic(InventoryWidget, &UMainInventoryWidget::UpdateInventoryUI);
				InventoryComponent->OnItemRemoved.AddDynamic(InventoryWidget, &UMainInventoryWidget::UpdateInventoryUI_Remove);
				InventoryComponent->OnInventorySizeIncreased.AddDynamic(InventoryWidget, &UMainInventoryWidget::SetInventoryGridMaxSlotCount);
			}
		}
	}


}

//void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//
//	FInputActionBinding& PauseBinding =
//		PlayerInputComponent->BindAction("PauseGame", IE_Pressed, this, &ABaseCharacter::HandlePausePressed);
//	PauseBinding.bExecuteWhenPaused = true;
//
//	FInputActionBinding& QuitBinding =
//		PlayerInputComponent->BindAction("QuitGame", IE_Pressed, this, &ABaseCharacter::QuitGameFromPause);
//	QuitBinding.bExecuteWhenPaused = true;
//}

//void ABaseCharacter::ApplyDamage(float DamageAmount)
//{
//	if (bIsDead || DamageAmount <= 0.0f)
//	{
//		return;
//	}
//
//	const float OldHealth = CurrentHealth;
//	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
//
//	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage. Health: %f -> %f"),
//		*GetName(), DamageAmount, OldHealth, CurrentHealth);
//
//	if (CurrentHealth <= 0.0f)
//	{
//		HandleDeath();
//	}
//}
//
//void ABaseCharacter::Heal(float HealAmount)
//{
//	if (bIsDead || HealAmount <= 0.0f)
//	{
//		return;
//	}
//
//	const float OldHealth = CurrentHealth;
//	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
//
//	UE_LOG(LogTemp, Warning, TEXT("%s healed %f. Health: %f -> %f"),
//		*GetName(), HealAmount, OldHealth, CurrentHealth);
//}
//
void ABaseCharacter::HandleDeath()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	UE_LOG(LogTemp, Warning, TEXT("%s died."), *GetName());

	if (GetCharacterMovement())
	{
		GetCharacterMovement()->StopMovementImmediately();
		GetCharacterMovement()->DisableMovement();
	}

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	HideHealthBarHUD();
	//ShowGameOverOverlay();
	ShowGameOverMenu();
	OnDeathBP();
}

void ABaseCharacter::ShowGameOverOverlay()
{
	if (bGameOverOverlayShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	GameOverSlateWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
				.Color(FLinearColor(0.f, 0.f, 0.f, 0.7f))
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(10.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("GAME OVER")))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 40))
						.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								RestartLevel();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Restart")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								Respawn();
								HideGameOverOverlay();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Respawn")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								QuitGameFromPause();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Quit to Desktop")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
		];

	GEngine->GameViewport->AddViewportWidgetContent(GameOverSlateWidget.ToSharedRef());
	bGameOverOverlayShown = true;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void ABaseCharacter::HideGameOverOverlay()
{
	if (!bGameOverOverlayShown)
	{
		return;
	}
	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}
	if (GameOverSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(GameOverSlateWidget.ToSharedRef());
		GameOverSlateWidget.Reset();
	}
	bGameOverOverlayShown = false;
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}
}

void ABaseCharacter::HandlePausePressed()
{
	UE_LOG(LogTemp, Warning, TEXT("Pause key pressed. bPauseOverlayShown = %s"),
		bPauseOverlayShown ? TEXT("true") : TEXT("false"));

	/*if (bIsDead)
	{
		return;
	}*/

	/*if (bPauseOverlayShown)
	{
		HidePauseOverlay();
	}
	else
	{
		ShowPauseOverlay();
	}*/
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null."));
		return;
	}

	if (PlayerController->IsPaused())
	{
		ResumeGame();
		return;
	}

	if (!PauseMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PauseMenuClass is not assigned."));
		return;
	}

	if (!PauseMenuWidget)
	{
		PauseMenuWidget = CreateWidget<UPauseMenuWidget>(GetWorld(), PauseMenuClass);
		PauseMenuWidget->AddToViewport(2);
	}


	if (!PauseMenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create pause menu."));
		return;
	}

	/*if (PlayerHud)
	{
		PlayerHud->SetVisibility(ESlateVisibility::Collapsed);
	}*/

	FInputModeGameAndUI InputMode;
	//InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock
	);

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetPause(true);
}

void ABaseCharacter::AttackingAnim()
{
	if (!CharacterAnimationComponent)
	{
		if (weapon)
		{
			weapon->ActionStopped();
		}
		return;
	}

	const bool bPlayedAnimation =
		CharacterAnimationComponent->PlayActionAnimation(
			ECharacterActionAnimationType::Attack
		);

	// If this character has no Attack animation assigned, do not leave
	// the weapon waiting for an animation-end callback that will never fire.
	if (!bPlayedAnimation && weapon)
	{
		weapon->ActionStopped();
	}
}

void ABaseCharacter::HandleActionAnimationEnded(
	ECharacterActionAnimationType AnimationType,
	bool bInterrupted
)
{
	if (AnimationType == ECharacterActionAnimationType::Attack)
	{
		AttackAnimEnded();
	}
}

void ABaseCharacter::AttackAnimEnded()
{
	if (weapon)
	{
		weapon->ActionStopped();
	}
}

void ABaseCharacter::ResumeGame() {
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("ResumeGame: PlayerController is null."));
		return;
	}


	PlayerController->SetPause(false);

	if (PauseMenuWidget)
	{
		PauseMenuWidget->RemoveFromParent();
		PauseMenuWidget = nullptr;
	}

	/*if (PlayerHud)
	{
		PlayerHud->SetVisibility(ESlateVisibility::Visible);
	}*/

	FInputModeGameOnly InputMode;
	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(false);
}

void ABaseCharacter::OpenOptionsMenu()
{
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null."));
		return;
	}

	if (!OptionsMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("OptionsMenuClass is not assigned."));
		return;
	}

	if (!OptionsMenuWidget)
	{
		OptionsMenuWidget = CreateWidget<UOptionsMenuWidget>(GetWorld(), OptionsMenuClass);
		OptionsMenuWidget->ShowOptionsMenu();
	}


	if (!OptionsMenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create options menu."));
		return;
	}

	PauseMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void ABaseCharacter::PreviousMenu()
{
	if (OptionsMenuWidget)
	{
		OptionsMenuWidget->RemoveFromParent();
		OptionsMenuWidget = nullptr;
		if (PauseMenuWidget)
		{
			PauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else if (PauseMenuWidget)
	{
		ResumeGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No menu to go back to."));
	}

}

void ABaseCharacter::ShowPauseOverlay()
{
	if (bPauseOverlayShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, true);
	}

	TSharedRef<SOverlay> PauseOverlay =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
				.Color(FLinearColor(0.f, 0.f, 0.f, 0.5f))
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(10.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("Paused")))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 32))
						.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								HidePauseOverlay();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Resume")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								RestartLevel();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Restart")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								ShowControlsOverlay();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Controls")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(5.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								QuitGameFromPause();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Quit to Desktop")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
		];

	PauseSlateWidget = PauseOverlay;
	GEngine->GameViewport->AddViewportWidgetContent(PauseOverlay);

	bPauseOverlayShown = true;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void ABaseCharacter::HidePauseOverlay()
{
	if (!bPauseOverlayShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	if (PauseSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(PauseSlateWidget.ToSharedRef());
		PauseSlateWidget.Reset();
	}

	bPauseOverlayShown = false;

	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, false);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetPause(false);

		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	UE_LOG(LogTemp, Warning, TEXT("HidePauseOverlay: game should now be unpaused."));
}

void ABaseCharacter::ShowControlsOverlay()
{
	if (bControlsOverlayShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	TSharedRef<SOverlay> ControlsOverlay =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
				.Color(FLinearColor(0.f, 0.f, 0.f, 0.7f))
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.Padding(FMargin(10.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Top)
				[
					SNew(STextBlock)
						.Text(FText::FromString(TEXT("Controls")))
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 32))
						.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot()
				.Padding(FMargin(40.f, 20.f, 40.f, 10.f))
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				[
					SNew(SScrollBox)
						+ SScrollBox::Slot()
						[
							SNew(STextBlock)
								.Text(FText::FromString(
									TEXT("Movement:\n")
									TEXT("  W / A / S / D - Move\n")
									TEXT("  Mouse - Look\n\n")
									TEXT("  SPACE - Jump\n\n")
									TEXT("Combat & Interaction:\n")
									TEXT("  Left Mouse Button - Attack\n")
									TEXT("  E - Equip item\n")
									TEXT("  F - Drop item\n\n")
									TEXT("Pause / Restart / Quit:\n")
									TEXT("  M - Pause / Unpause\n")
									TEXT("  J - Quit (when paused or on Game Over)\n")
								))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
			+ SVerticalBox::Slot()
				.Padding(FMargin(10.f))
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Bottom)
				[
					SNew(SButton)
						.OnClicked_Lambda([this]()
							{
								HideControlsOverlay();
								return FReply::Handled();
							})
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Back")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
								.ColorAndOpacity(FLinearColor::White)
						]
				]
		];

	ControlsSlateWidget = ControlsOverlay;
	GEngine->GameViewport->AddViewportWidgetContent(ControlsOverlay);

	bControlsOverlayShown = true;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void ABaseCharacter::HideControlsOverlay()
{
	if (!bControlsOverlayShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	if (ControlsSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(ControlsSlateWidget.ToSharedRef());
		ControlsSlateWidget.Reset();
	}

	bControlsOverlayShown = false;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void ABaseCharacter::ShowStartScreen()
{
	if (bStartScreenShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, true);
	}

	const FLinearColor FullBlack = FLinearColor(0.f, 0.f, 0.f, 1.f);
	const FLinearColor Orange = FLinearColor(1.f, 0.5f, 0.f, 1.f);

	TSharedRef<SOverlay> StartOverlay =
		SNew(SOverlay)
		+ SOverlay::Slot()
		[
			SNew(SColorBlock)
				.Color(FullBlack)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBorder)
				.BorderBackgroundColor(FullBlack)
				.Padding(FMargin(30.f))
				.BorderImage(FCoreStyle::Get().GetBrush("GenericWhiteBox"))
				.ForegroundColor(Orange)
				[
					SNew(SVerticalBox)
						+ SVerticalBox::Slot()
						.Padding(FMargin(0.f, 0.f, 0.f, 10.f))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Top)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Clockwork Horrors")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 32))
								.ColorAndOpacity(Orange)
						]
						+ SVerticalBox::Slot()
						.Padding(FMargin(0.f, 0.f, 0.f, 10.f))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Top)
						[
							SNew(STextBlock)
								.Text(FText::FromString(TEXT("Controls")))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 24))
								.ColorAndOpacity(Orange)
						]
						+ SVerticalBox::Slot()
						.Padding(FMargin(0.f, 10.f, 0.f, 20.f))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(STextBlock)
								.Text(FText::FromString(
									TEXT("Movement: W/A/S/D, Look: Mouse, Jump: SPACE\n")
									TEXT("Attack: LMB, Equip: E, Drop Item: F, Pause: M, Quit: J\n")
								))
								.Font(FCoreStyle::GetDefaultFontStyle("Regular", 15))
								.ColorAndOpacity(Orange)
								.Justification(ETextJustify::Center)
						]
					+ SVerticalBox::Slot()
						.Padding(FMargin(0.f, 10.f, 0.f, 5.f))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
								.OnClicked_Lambda([this]()
									{
										HideStartScreen();
										return FReply::Handled();
									})
								[
									SNew(STextBlock)
										.Text(FText::FromString(TEXT("Start Game")))
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
										.ColorAndOpacity(Orange)
								]
						]
					+ SVerticalBox::Slot()
						.Padding(FMargin(0.f, 5.f, 0.f, 0.f))
						.HAlign(HAlign_Center)
						.VAlign(VAlign_Center)
						[
							SNew(SButton)
								.OnClicked_Lambda([this]()
									{
										QuitGameFromPause();
										return FReply::Handled();
									})
								[
									SNew(STextBlock)
										.Text(FText::FromString(TEXT("Quit Game")))
										.Font(FCoreStyle::GetDefaultFontStyle("Regular", 20))
										.ColorAndOpacity(Orange)
								]
						]
				]
		];

	StartSlateWidget = StartOverlay;
	GEngine->GameViewport->AddViewportWidgetContent(StartOverlay);

	bStartScreenShown = true;

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;

		FInputModeGameAndUI InputMode;
		InputMode.SetHideCursorDuringCapture(false);
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void ABaseCharacter::HideStartScreen()
{
	if (!bStartScreenShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	if (StartSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(StartSlateWidget.ToSharedRef());
		StartSlateWidget.Reset();
	}

	bStartScreenShown = false;

	UWorld* World = GetWorld();
	if (World)
	{
		UGameplayStatics::SetGamePaused(World, false);
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->SetPause(false);
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	ShowHealthBarHUD();
}

void ABaseCharacter::InventoryHUD()
{
	//UE_LOG(LogTemp, Warning, TEXT("ABaseCharacter::InventoryHUD called. InventoryWidget is %s"), InventoryWidget ? TEXT("valid") : TEXT("null"));

	if (InventoryWidget)
	{
		isInInventory = !isInInventory;

		if (isInInventory)
		{
			InventoryWidget->ShowInventory(isInInventory);
		}
		else if (!isInInventory)
		{
			InventoryWidget->ShowInventory(isInInventory);
		}

	}
}

float ABaseCharacter::GetHealthPercent() const
{
	if (!HealthComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthComponent is null in GetHealthPercent()"));
		return 0.0f;
	}

	if (HealthComponent->GetMaxHealth() <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("MaxHealth is zero or negative in GetHealthPercent()"));
		return 0.0f;
	}

	return HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth();

	/*if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}
	return CurrentHealth / MaxHealth;*/
}

FText ABaseCharacter::GetHealthText() const
{
	if (!HealthComponent)
	{
		return FText::FromString(TEXT("Health Component not found"));
	}

	return FText::FromString(FString::Printf(TEXT("%d / %d"),
		FMath::RoundToInt(HealthComponent->GetCurrentHealth()), FMath::RoundToInt(HealthComponent->GetMaxHealth())));

	/*return FText::FromString(FString::Printf(TEXT("%d / %d"),
		FMath::RoundToInt(CurrentHealth), FMath::RoundToInt(MaxHealth)));*/
}

void ABaseCharacter::ShowHealthBarHUD()
{
	if (bHealthBarShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	HealthBarSlateWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Top)
		.Padding(FMargin(30.f, 30.f, 0.f, 0.f))
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.f, 0.f, 0.f, 4.f))
				[
					SNew(STextBlock)
						.Text_Lambda([this]() { return GetHealthText(); })
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
						.WidthOverride(250.f)
						.HeightOverride(22.f)
						[
							SNew(SProgressBar)
								.Percent_Lambda([this]() { return TOptional<float>(GetHealthPercent()); })
								.FillColorAndOpacity(FLinearColor(0.85f, 0.1f, 0.1f, 1.f))
								.BackgroundImage(FCoreStyle::Get().GetBrush("ProgressBar.Background"))
						]
				]
		];

	GEngine->GameViewport->AddViewportWidgetContent(HealthBarSlateWidget.ToSharedRef());
	bHealthBarShown = true;
}

void ABaseCharacter::HideHealthBarHUD()
{
	if (!bHealthBarShown)
	{
		return;
	}

	if (GEngine && GEngine->GameViewport && HealthBarSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(HealthBarSlateWidget.ToSharedRef());
		HealthBarSlateWidget.Reset();
	}

	bHealthBarShown = false;
}

void ABaseCharacter::ShowGameOverMenu()
{
	APlayerController* PlayerController =
		Cast<APlayerController>(GetController());

	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null."));
		return;
	}

	if (!GameOverMenuClass)
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverMenuClass is not assigned."));
		return;
	}

	if (!GameOverMenuWidget)
	{
		GameOverMenuWidget = CreateWidget<UGameOverWidget>(GetWorld(), GameOverMenuClass);
		GameOverMenuWidget->AddToViewport(2);
	}


	if (!GameOverMenuWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create game over menu."));
		return;
	}

	/*if (PlayerHud)
	{
		PlayerHud->SetVisibility(ESlateVisibility::Collapsed);
	}*/

	FInputModeUIOnly InputMode;
	//InputMode.SetWidgetToFocus(PauseMenuWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(
		EMouseLockMode::DoNotLock
	);

	PlayerController->SetInputMode(InputMode);
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetPause(true);
}

void ABaseCharacter::RestartLevel()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (bPauseOverlayShown && GEngine && GEngine->GameViewport && PauseSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(PauseSlateWidget.ToSharedRef());
		PauseSlateWidget.Reset();
		bPauseOverlayShown = false;
	}

	if (bGameOverOverlayShown && GEngine && GEngine->GameViewport && GameOverSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(GameOverSlateWidget.ToSharedRef());
		GameOverSlateWidget.Reset();
		bGameOverOverlayShown = false;
	}

	if (bControlsOverlayShown && GEngine && GEngine->GameViewport && ControlsSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(ControlsSlateWidget.ToSharedRef());
		ControlsSlateWidget.Reset();
		bControlsOverlayShown = false;
	}

	if (bStartScreenShown && GEngine && GEngine->GameViewport && StartSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(StartSlateWidget.ToSharedRef());
		StartSlateWidget.Reset();
		bStartScreenShown = false;
	}

	if (bHealthBarShown && GEngine && GEngine->GameViewport && HealthBarSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(HealthBarSlateWidget.ToSharedRef());
		HealthBarSlateWidget.Reset();
		bHealthBarShown = false;
	}

	UGameplayStatics::SetGamePaused(World, false);

	APlayerController* PC = World->GetFirstPlayerController();
	if (PC)
	{
		PC->SetPause(false);
		PC->bShowMouseCursor = false;
		PC->SetInputMode(FInputModeGameOnly());
	}

	const FString CurrentMapString = UGameplayStatics::GetCurrentLevelName(World, /*bRemovePrefixString=*/true);
	const FName CurrentLevelName(*CurrentMapString);

	UGameplayStatics::OpenLevel(World, CurrentLevelName);
}

void ABaseCharacter::Respawn()
{
	if (!CheckpointManager)
	{
		UE_LOG(LogTemp, Error, TEXT("CheckpointManager is null in Respawn()"));
		return;
	}

	//UE_LOG(LogTemp, Warning, TEXT("Respawning player..."));

	CheckpointManager->CallFunctionByNameWithArguments(TEXT("RespawnPlayer"), *GLog, nullptr, true);
}

void ABaseCharacter::QuitGameFromPause()
{
	if (!bPauseOverlayShown && /*!bIsDead &&*/ !bStartScreenShown)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	UGameplayStatics::SetGamePaused(World, false);
	PC->SetPause(false);

	UKismetSystemLibrary::QuitGame(World, PC, EQuitPreference::Quit, false);
}

void ABaseCharacter::Attack()
{
	if (bHasWeapon)
	{
		weapon->Attack();

	}
}
void ABaseCharacter::Reload()
{
	if (bHasWeapon)
	{
		if (ABaseBlaster* blaster = Cast<ABaseBlaster>(weapon))
		{
			UE_LOG(LogTemp, Warning, TEXT("Called Reload Ammo"));
			blaster->reloadAmmo();
		}
	}
}