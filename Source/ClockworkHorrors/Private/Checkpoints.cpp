// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Checkpoints.h"
#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "CheckpointManager.h"
#include "ClockworkPlayerController.h"

ACheckpoints::ACheckpoints()
{
	bIsActive = false;
	PlayerActorToSpawn = nullptr;
	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CheckpointMesh"));
	//CheckpointCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CheckpointCollision"));
}

void ACheckpoints::BeginPlay()
{
    Super::BeginPlay();
}

void ACheckpoints::BoundOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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
    if(bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Checkpoint %s has already been reached."), *GetName());
        return;
	}

    bIsActive = true;

	CheckpointManager->ChangeCurrentCheckpoint(this);
}

void ACheckpoints::DeactivateCheckpoint()
{
    if(!bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("Checkpoint %s is not active."), *GetName());
        return;
	}
	bIsActive = false;
}

void ACheckpoints::SpawnPlayerAtCheckpoint()
{
    if (!PlayerActorClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnPlayerAtCheckpoint: PlayerActorClass is null"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

    AClockworkPlayerController* ClockworkPC = Cast<AClockworkPlayerController>(PC);
    if (!ClockworkPC) return;

	UE_LOG(LogTemp, Warning, TEXT("Spawning player at checkpoint: %s"), *GetName());

	ABaseCharacter* Player = Cast<ABaseCharacter>(ClockworkPC->GetPawn());
	Player->TeleportTo(GetActorLocation(), GetActorRotation());
    Player->Reset();

    /*
    UWorld* World = GetWorld();
    if (!World) return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC) return;

	AClockworkPlayerController* ClockworkPC = Cast<AClockworkPlayerController>(PC);
	if (!ClockworkPC) return;

    if (APawn* OldPawn = ClockworkPC->GetPawn())
    {
        ClockworkPC->UnPossess();
        OldPawn->Destroy();
    }

	UE_LOG(LogTemp, Warning, TEXT("Spawning new pawn at checkpoint: %s"), *GetName());

    // 3. Spawn new pawn at checkpoint transform
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APawn* NewPawn = World->SpawnActor<APawn>(PlayerActorClass, GetActorLocation(), GetActorRotation(), Params);
    if (NewPawn)
    {
        ClockworkPC->Possess(NewPawn);
    }
    */
}
