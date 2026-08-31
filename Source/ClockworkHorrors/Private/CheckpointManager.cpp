// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "CheckpointManager.h"
#include "Checkpoints.h"
#include "Components/BoxComponent.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "ClockworkPlayerController.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ACheckpointManager::ACheckpointManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	CurrentActiveCheckpoint = nullptr;
}

// Called when the game starts or when spawned
void ACheckpointManager::BeginPlay()
{
	Super::BeginPlay();
}

void ACheckpointManager::RespawnPlayerAtStart()
{
	if (CurrentActiveCheckpoint)
	{
		CurrentActiveCheckpoint->ActivateCheckpoint();
	}

	if (!PlayerStart)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckpointManager: No PlayerStart found in the level."));
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World) return;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(World, 0);
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckpointManager: No player character found."));
		return;
	}

	PlayerCharacter->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), false, true);

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(PlayerCharacter);
	if (BaseCharacter)
	{
		BaseCharacter->ResetCharacterForRespawn(); // handles bIsDead, health, movement mode, hidden/collision/tick, HUD
		
		PlayerCharacter->SetActorHiddenInGame(false);
		PlayerCharacter->SetActorEnableCollision(true);
		PlayerCharacter->SetActorTickEnabled(true);
		/*BaseCharacter->HideGameOverMenu();*/
	}
	else
	{
		// Fallback if not a ABaseCharacter for some reason
		PlayerCharacter->SetActorHiddenInGame(false);
		PlayerCharacter->SetActorEnableCollision(true);
		PlayerCharacter->SetActorTickEnabled(true);
	}

	APlayerController* PC = World->GetFirstPlayerController();
	if (AClockworkPlayerController* ClockworkPC = Cast<AClockworkPlayerController>(PC))
	{
		//ClockworkPC->HideGameOverScreen(); // this should call SetPause(false), restore input mode, hide cursor
	}
	else if (PC)
	{
		PC->SetPause(false); // fallback
	}
}

void ACheckpointManager::ChangeCurrentCheckpoint(ACheckpoints* NewCheckpoint)
{
	// Check if the new checkpoint is valid and not the same as the current active checkpoint
	if(!NewCheckpoint && NewCheckpoint == CurrentActiveCheckpoint)
	{
		return;
	}
	
	// Check if the new checkpoint has already been activated
	if(OldCheckpointsArray.Contains(NewCheckpoint))
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckpointManager: Checkpoint %s has already been activated."), *NewCheckpoint->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("CheckpointManager: Checkpoint Changed"));

	// Deactivate the current active checkpoint if it exists
	if(CurrentActiveCheckpoint)
	{
		CurrentActiveCheckpoint->DeactivateCheckpoint();
		OldCheckpointsArray.Add(CurrentActiveCheckpoint);
	}
	CurrentActiveCheckpoint = NewCheckpoint;
}

void ACheckpointManager::RespawnPlayer()
{
	if(!CurrentActiveCheckpoint)
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckpointManager: No active checkpoint found. Respawning player at start."));
		RespawnPlayerAtStart();
		return;
	}

	CurrentActiveCheckpoint->ActivateCheckpoint();
}

// Called every frame
void ACheckpointManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

