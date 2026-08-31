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
#include "Utils/ExperienceComponent.h"

#include "InputCoreTypes.h"
#include "CheckpointManager.h"
#include "BaseBlaster.h"
#include "Staff.h"
#include "UI/PauseMenuWidget.h"
#include "UI/GameOverWidget.h"
#include "UI/OptionsMenuWidget.h"
#include "UI/MainInventoryWidget.h"
#include "Components/CharacterAnimationComponent.h"
#include <WeaponPickup.h>
#include "WeaponSlots.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Utils/PlayerSaveGame.h"
#include "Interfaces/CompanionInterface.h"
#include "SealedDoor.h"
#include "Actors/TrapActivator.h"
ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Components
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	WeaponSlots = CreateDefaultSubobject<UWeaponSlots>(TEXT("WeaponSlots"));
	ExperienceComponent = CreateDefaultSubobject<UExperienceComponent>(TEXT("ExperienceComponent"));

	MiniMapSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("MiniMapSpringArm"));
	MiniMapSpringArm->SetupAttachment(RootComponent);
	MiniMapSpringArm->TargetArmLength = 1000.0f;
	MiniMapSpringArm->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	MiniMapSpringArm->bInheritPitch = false;
	MiniMapSpringArm->bInheritYaw = false;
	MiniMapSpringArm->bInheritRoll = false;
	MiniMapCaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MiniMapCaptureComponent"));
	MiniMapCaptureComponent->SetupAttachment(MiniMapSpringArm);
}

void ABaseCharacter::TryPickupInteract()
{
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		ObjectInteract(Actor);
		ItemEquip(Actor);
	}
}

void ABaseCharacter::ObjectInteract(AActor* Actor)
{
	if (ASealedDoor* Door = Cast<ASealedDoor>(Actor))
	{
		Door->OnInteract();
	}

	if(ATrapActivator* TrapActivator = Cast<ATrapActivator>(Actor))
	{
		TrapActivator->OnInteract();
	}
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	bIsDead = false;
	if (HealthComponent)
	{
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
		//InventoryComponent->OnWeaponPickedUp.AddDynamic(this, &ABaseCharacter::EquipPickupWeapon);
	}


	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		MiniMapWidget = CreateWidget(GetWorld()->GetFirstPlayerController(), MiniMapClass);
		if (MiniMapWidget)
		{
			MiniMapWidget->AddToViewport();
			MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
		}

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

	if (Companion)
	{
		FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f + FVector(0.0f, 100.0f, 0.0f);
		FRotator SpawnRotation = GetActorRotation();


		FHitResult HitResult;
		FVector TraceStart = SpawnLocation;
		FVector TraceEnd = SpawnLocation - FVector(0.0f, 0.0f, 1000.0f);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
		{

			SpawnLocation = HitResult.Location + FVector(0.0f, 0.0f, 50.0f);
		}

		AActor* Comp = GetWorld()->SpawnActor<AActor>(Companion, SpawnLocation, SpawnRotation);
		if (Comp)
		{
			CompanionInstance = Cast<ICompanionInterface>(Comp);
		}
	}
	OnLevelChange.AddDynamic(this, &ABaseCharacter::SavePlayerData);
	GetWorld()->OnWorldBeginPlay.AddUObject(this, &ABaseCharacter::LoadPlayerData);
}




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

	HideHuds();
	ShowGameOverMenu();
	OnDeathBP();
}

void ABaseCharacter::ResetCharacterForRespawn()
{
	if (!HealthComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("HealthComponent is missing on %s"),
			*GetName()
		);
		return;
	}

	HealthComponent->ResetHealth();
	bIsDead = false;

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	if (UCharacterMovementComponent* MovementComponent =
		GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->SetMovementMode(MOVE_Walking);
	}

	if (GameOverMenuWidget)
	{
		GameOverMenuWidget->RemoveFromParent();
		GameOverMenuWidget = nullptr;
	}

	HideGameOverOverlay();

	if (APlayerController* PlayerController =
		Cast<APlayerController>(GetController()))
	{
		PlayerController->SetPause(false);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());
	}
	ShowHuds();
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

// .cpp
float ABaseCharacter::GetExperiencePercent() const
{
	if (!ExperienceComponent)
	{
		return 0.0f;
	}

	return ExperienceComponent->GetExperiencePercent();
}

FText ABaseCharacter::GetExperienceText() const
{
	if (!ExperienceComponent)
	{
		return FText::FromString(TEXT("Experience Component not found"));
	}

	return FText::FromString(FString::Printf(
		TEXT("Lv. %d   %d / %d XP"),
		ExperienceComponent->GetLevel(),
		FMath::RoundToInt(ExperienceComponent->GetExperiencePoints()),
		FMath::RoundToInt(ExperienceComponent->GetMaxExperiencePoints())
	));
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
									TEXT("  I - Inventory\n")
									TEXT("Combat & Interaction:\n")
									TEXT("  Left Mouse Button - Attack\n")
									TEXT("  Scroll wheel - Change spell\n")
									TEXT("  E - Equip item\n")
									TEXT("  F - Drop item\n\n")
									TEXT("Pause / Restart / Quit:\n")
									TEXT("  M - Pause / Unpause\n")
									
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
										.Text(FText::FromString(TEXT("Start Level")))
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

	static FSlateColorBrush HealthBarBackgroundBrush(FLinearColor(0.35f, 0.2f, 0.0f, 1.0f));

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
								.FillColorAndOpacity(FLinearColor(1.0f, 0.65f, 0.0f, 1.f))
								.BackgroundImage(&HealthBarBackgroundBrush)
						]
				]
		];

	GEngine->GameViewport->AddViewportWidgetContent(HealthBarSlateWidget.ToSharedRef());
	bHealthBarShown = true;
}


void ABaseCharacter::HideStartScreen()
{
	if (!bStartScreenShown)
	{
		return;
	}

	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	if (StartSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(StartSlateWidget.ToSharedRef());
	}

	StartSlateWidget.Reset();
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

	ShowHuds();
}

void ABaseCharacter::InventoryHUD()
{

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

void ABaseCharacter::HideHuds()
{
	HideMiniMapHUD();
	HideHealthBarHUD();
	HideReticleHUD();
	HideExperienceHUD();
}

void ABaseCharacter::ShowHuds()
{
	ShowMiniMapHUD();
	ShowHealthBarHUD();
	ShowReticleHUD();
	ShowExperienceHUD();
}

float ABaseCharacter::GetHealthPercent() const
{
	if (!IsValid(HealthComponent))
	{
		return 0.0f;
	}

	const float CurrentHealth = HealthComponent->GetCurrentHealth();
	const float MaxHealth = HealthComponent->GetMaxHealth();

	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}

	return FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
}

FText ABaseCharacter::GetHealthText() const
{
	if (!HealthComponent)
	{
		return FText::FromString(TEXT("Health Component not found"));
	}

	return FText::FromString(FString::Printf(TEXT("%d / %d"),
		FMath::RoundToInt(HealthComponent->GetCurrentHealth()), FMath::RoundToInt(HealthComponent->GetMaxHealth())));

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

void ABaseCharacter::ShowReticleHUD()
{
	if (bReticleShown)
	{
		return;
	}

	if (!(GEngine && GEngine->GameViewport))
	{
		return;
	}

	const FLinearColor ReticleColor = FLinearColor::White;

	ReticleSlateWidget =
		SNew(SOverlay)

		// Horizontal bar
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
				.WidthOverride(24.0f)
				.HeightOverride(2.0f)
				[
					SNew(SColorBlock)
						.Color(ReticleColor)
				]
		]

		// Vertical bar
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SBox)
				.WidthOverride(2.0f)
				.HeightOverride(24.0f)
				[
					SNew(SColorBlock)
						.Color(ReticleColor)
				]
		];

	GEngine->GameViewport->AddViewportWidgetContent(
		ReticleSlateWidget.ToSharedRef()
	);

	bReticleShown = true;
}

void ABaseCharacter::HideReticleHUD()
{
	if (!bReticleShown)
	{
		return;
	}

	if (GEngine && GEngine->GameViewport && ReticleSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(
			ReticleSlateWidget.ToSharedRef()
		);

		ReticleSlateWidget.Reset();
	}

	bReticleShown = false;
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

	if (bReticleShown && GEngine && GEngine->GameViewport && ReticleSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(
			ReticleSlateWidget.ToSharedRef()
		);

		ReticleSlateWidget.Reset();
		bReticleShown = false;
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

float ABaseCharacter::GetCurrentHealth() const
{
	return HealthComponent ? HealthComponent->GetCurrentHealth() / HealthComponent->GetMaxHealth() : 0.0f;
}

void ABaseCharacter::UpdateCompanionTarget(AActor* NewTarget, float damage)
{
	if (CompanionInstance)
	{
		CompanionInstance->UpdateTarget(NewTarget, damage);
	}
}

void ABaseCharacter::HealPlayer(float HealAmount)
{
	if (HealthComponent)
	{
		HealthComponent->Heal(HealAmount);
	}
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
	if (weapon)
	{
		weapon->Attack();

	}
}

void ABaseCharacter::StopAttack()
{
	if (AStaff* Staff = Cast<AStaff>(weapon))
	{
		Staff->ReleaseSpellCharge();
	}
}
void ABaseCharacter::Reload()
{
	if (weapon)
	{
		if (ABaseBlaster* blaster = Cast<ABaseBlaster>(weapon))
		{
			UE_LOG(LogTemp, Warning, TEXT("Called Reload Ammo"));
			blaster->reloadAmmo();
		}
	}
}

void ABaseCharacter::EquipWeaponSlot(int32 SlotNumber)
{
	if (!WeaponSlots || !InventoryComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("WeaponSlots or InventoryComponent is null"));
		return;
	}

	if (!WeaponSlots->HasWeaponInSlot(SlotNumber))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("Slot %d is empty"), SlotNumber));
		return;
	}

	if (weapon && weapon->ActionHappening)
	{
		return;
	}

	ABaseWeapon* OldWeapon = weapon;
	if (OldWeapon)
	{
		if (UWeaponPickup* TempPickup = OldWeapon->GetComponentByClass<UWeaponPickup>())
		{
			if (TempPickup->ItemDataAsset)
			{
				FInventorySlotEntry TempSlot = InventoryComponent->GetItem(TempPickup->ItemDataAsset->ItemName);

				TempSlot.bIsEquipped = false;

				InventoryComponent->OnItemDataAdded.Broadcast(TempSlot, TempSlot.CurrentBind, true);
			}
		}
	}

	WeaponSlots->EquipSlot(SlotNumber);

	if (!weapon)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("EquipSlot(%d) ran, but weapon is still null"), SlotNumber));
		return;
	}

	if (UWeaponPickup* BasePickup = weapon->GetComponentByClass<UWeaponPickup>())
	{
		if (BasePickup->ItemDataAsset)
		{
			FInventorySlotEntry Slot = InventoryComponent->GetItem(BasePickup->ItemDataAsset->ItemName);

			Slot.bIsEquipped = true;

			InventoryComponent->OnItemDataAdded.Broadcast(Slot, Slot.CurrentBind, true);
		}
	}
}

void ABaseCharacter::ItemEquip(AActor* Actor)
{
	if (!IsValid(Actor))
	{
		return;
	}

	// Weapon pickup path.
	if (UWeaponPickup* WeaponPickup =
		Actor->FindComponentByClass<UWeaponPickup>())
	{
		if (WeaponPickup->IsStored())
		{
			return;
		}

		ABaseWeapon* BaseWeapon = Cast<ABaseWeapon>(Actor);
		if (weapon)
		{
			UWeaponPickup* tempPickup = weapon->GetComponentByClass<UWeaponPickup>();
			FInventorySlotEntry tempSlot = InventoryComponent->GetItem(tempPickup->ItemDataAsset->ItemName);
			tempSlot.bIsEquipped = false;
			InventoryComponent->OnItemDataAdded.Broadcast(tempSlot, tempSlot.CurrentBind, true);
		}
		WeaponPickup->HandleInteractPressed();

		if (!WeaponPickup->IsStored() ||
			WeaponPickup->GetStoredHolder() != this)
		{
			return;
		}

		if (WeaponPickup->ItemDataAsset)
		{
			if (IsValid(BaseWeapon))
			{
				WeaponPickup->ItemDataAsset->WeaponRef = BaseWeapon;
			}

			if (InventoryComponent)
			{
				InventoryComponent->AddItem(WeaponPickup->ItemDataAsset);
			}

			if (WeaponPickup->ItemDataAsset->WeaponClass)
			{
				weaponClass = WeaponPickup->ItemDataAsset->WeaponClass;
			}
		}

		if (IsValid(BaseWeapon))
		{
			BaseWeapon->ParentPawn = this;
			BaseWeapon->bPickedUp = true;
		}

		if (WeaponSlots)
		{

			const bool bAddedToWeaponSlots =
				WeaponSlots->AddWeapon(WeaponPickup);
			FInventorySlotEntry slot = InventoryComponent->GetItem(WeaponPickup->ItemDataAsset->ItemName);
			slot.bIsEquipped = true;
			InventoryComponent->OnItemDataAdded.Broadcast(slot, slot.CurrentBind, false);
			UE_LOG(
				LogTemp,
				Warning,
				TEXT("WeaponSlots: added %s to weapon slot %d: %s"),
				*GetNameSafe(BaseWeapon),
				WeaponPickup->GetInventorySlot(),
				bAddedToWeaponSlots ? TEXT("true") : TEXT("false")
			);
			return;
		}

		// Generic pickup path: health items and any non-weapon ABasePickup.
		if (ABasePickup* GenericPickup = Cast<ABasePickup>(Actor))
		{
			GenericPickup->OnInteract();
			return;
		}
	}

	// Generic pickup path: health items and any non-weapon ABasePickup.
	if (ABasePickup* GenericPickup = Cast<ABasePickup>(Actor))
	{
		GenericPickup->OnInteract();
		return;
	}
}

void ABaseCharacter::SavePlayerData()
{
	UPlayerSaveGame* saveData = UPlayerSaveGame::LoadPlayerData();
	if (saveData)
	{
		saveData->SavePlayerData(HealthComponent->GetCurrentHealth(), ExperienceComponent->GetExperiencePoints(), ExperienceComponent->GetLevel(), ExperienceComponent->GetSkillPoints(), InventoryComponent->GetInventory(), WeaponSlots->GetEquippedSlot());
	}
	HideHuds();
}

void ABaseCharacter::LoadPlayerData()
{
	UPlayerSaveGame* saveData = UPlayerSaveGame::LoadPlayerData();
	if (saveData)
	{
		ExperienceComponent->SetLevel(saveData->Level);
		ExperienceComponent->SetExperiencePoints(saveData->Experience);
		ExperienceComponent->SetSkillPoints(saveData->SkillPoints);
		HealthComponent->SetCurrentHealth(saveData->currentHealth);
		for (FInventorySlotEntry slot : saveData->Inventory)
		{
			if (slot.ItemData)
			{
				//InventoryComponent->AddItem(slot.ItemData);
				FActorSpawnParameters Params;
				Params.Instigator = this;
				Params.Owner = this;
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
					slot.ItemData->BlueprintClass,
					this->GetActorLocation(),
					this->GetActorRotation(),
					Params
				);
				if (SpawnedActor)
				{
					ItemEquip(SpawnedActor);
					if (Cast<ABaseBlaster>(SpawnedActor))
					{
						Cast<ABaseBlaster>(SpawnedActor)->setCurrAmmo(slot.ItemData->Ammo);
					}
				}
			}

		}
		EquipWeaponSlot(saveData->currentEquippedSlot);
	}
	bFinishedBeginPlay = true;
}


void ABaseCharacter::ShowExperienceHUD()
{
	if (bExperienceBarShown)
	{
		return;
	}

	if (!GEngine || !GEngine->GameViewport)
	{
		return;
	}

	static FSlateColorBrush ExperienceBarBackgroundBrush(FLinearColor(0.35f, 0.2f, 0.0f, 1.0f));

	ExperienceSlateWidget =
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Bottom)
		.Padding(FMargin(30.f, 0.f, 0.f, 30.f))
		[
			SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				.Padding(FMargin(0.f, 0.f, 0.f, 4.f))
				[
					SNew(STextBlock)
						.Text_Lambda([this]() { return GetExperienceText(); })
						.Font(FCoreStyle::GetDefaultFontStyle("Regular", 16))
						.ColorAndOpacity(FLinearColor::White)
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBox)
						.WidthOverride(250.f)
						.HeightOverride(16.f)
						[
							SNew(SProgressBar)
								.Percent_Lambda([this]() { return TOptional<float>(GetExperiencePercent()); })
								.FillColorAndOpacity(FLinearColor(0.1f, 0.6f, 0.9f, 1.f))
								.BackgroundImage(&ExperienceBarBackgroundBrush)
						]
				]
		];

	GEngine->GameViewport->AddViewportWidgetContent(ExperienceSlateWidget.ToSharedRef());
	bExperienceBarShown = true;
}

void ABaseCharacter::HideExperienceHUD()
{
	if (!bExperienceBarShown)
	{
		return;
	}

	if (GEngine && GEngine->GameViewport && ExperienceSlateWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(ExperienceSlateWidget.ToSharedRef());
	}

	ExperienceSlateWidget.Reset();
	bExperienceBarShown = false;
}
void ABaseCharacter::ShowMiniMapHUD()
{
	if (!MiniMapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget is null in ShowMiniMapHUD()"));
		return;
	}
	MiniMapWidget->SetVisibility(ESlateVisibility::Visible);
}
void ABaseCharacter::HideMiniMapHUD()
{
	if (!MiniMapWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("MiniMapWidget is null in HideMiniMapHUD()"));
		return;
	}
	MiniMapWidget->SetVisibility(ESlateVisibility::Hidden);
}



void ABaseCharacter::DropEquippedWeapon(FInventorySlotEntry slot)
{
	if (!WeaponSlots)
	{
		return;
	}
	if (slot.ItemData)
	{
		if (slot.ItemData->WeaponClass)
		{
			slot.bIsEquipped = false;
			InventoryComponent->RemoveItemByName(slot.GetItemDataName());
			UWeaponPickup* basePickup = slot.ItemData->WeaponRef->GetComponentByClass<UWeaponPickup>();
			WeaponSlots->DropEquippedWeapon(basePickup->InventorySlot);
		}

	}


}

