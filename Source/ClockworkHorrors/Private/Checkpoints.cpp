// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Checkpoints.h"

#include "BaseCharacter.h"
#include "CheckpointManager.h"
#include "ClockworkPlayerController.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include <CodeGameInstance.h>

ACheckpoints::ACheckpoints()
{
    bIsActive = false;
    PlayerActorToSpawn = nullptr;

    CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));

    // CheckpointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointCollision"));
}

void ACheckpoints::BeginPlay()
{
    Super::BeginPlay();
}

void ACheckpoints::BoundOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (OtherActor && OtherActor->IsA(ABaseCharacter::StaticClass()))
    {
        CheckpointReached();
    }
}

void ACheckpoints::ActivateCheckpoint()
{
    SpawnPlayerAtCheckpoint();
}

void ACheckpoints::CheckpointReached()
{
    if (bIsActive)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Checkpoint %s has already been reached."),
            *GetName());
        return;
    }

    bIsActive = true;

    // Only transition if this specific checkpoint was configured as a level exit.
    if (bLoadsNewLevel)
    {
        LoadNextLevel();
        return;
    }

    if (CheckpointManager)
    {
        CheckpointManager->ChangeCurrentCheckpoint(this);
    }
    else
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Checkpoint %s has no CheckpointManager assigned."),
            *GetName());
    }
}

void ACheckpoints::DeactivateCheckpoint()
{
    if (!bIsActive)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Checkpoint %s is not active."),
            *GetName());
        return;
    }

    bIsActive = false;
}

void ACheckpoints::LoadNextLevel()
{
    if (LevelToLoad.IsNone())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("Checkpoint %s cannot load a level because LevelToLoad is not set."),
            *GetName());
        return;
    }

    UE_LOG(LogTemp, Log,
        TEXT("Checkpoint %s loading level: %s."),
        *GetName(),
        *LevelToLoad.ToString());
    UCodeGameInstance* instance = Cast<UCodeGameInstance>(GetGameInstance());
    int index = instance->GameLevels.Find(LevelToLoad);
    instance->LoadLevelSafe(index);
    //UGameplayStatics::OpenLevel(this, LevelToLoad, true);
}

void ACheckpoints::SpawnPlayerAtCheckpoint()
{
    if (!PlayerActorClass)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("SpawnPlayerAtCheckpoint: PlayerActorClass is null"));
        return;
    }

    UWorld* World = GetWorld();

    if (!World)
    {
        return;
    }

    ACharacter* PlayerCharacter =
        UGameplayStatics::GetPlayerCharacter(World, 0);

    if (!PlayerCharacter)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("CheckpointManager: No player character found."));
        return;
    }

    PlayerCharacter->TeleportTo(
        GetActorLocation(),
        GetActorRotation(),
        false,
        true
    );

    ABaseCharacter* BaseCharacter =
        Cast<ABaseCharacter>(PlayerCharacter);

    if (BaseCharacter)
    {
        BaseCharacter->ResetCharacterForRespawn();

        PlayerCharacter->SetActorHiddenInGame(false);
        PlayerCharacter->SetActorEnableCollision(true);
        PlayerCharacter->SetActorTickEnabled(true);
    }
    else
    {
        PlayerCharacter->SetActorHiddenInGame(false);
        PlayerCharacter->SetActorEnableCollision(true);
        PlayerCharacter->SetActorTickEnabled(true);
    }
}