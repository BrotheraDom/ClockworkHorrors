#include "CompanionSpawner.h"
#include "CompanionCharacter.h" // ?? Includes CompanionCharacter header
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

ACompanionSpawner::ACompanionSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

ACompanionCharacter* ACompanionSpawner::TriggerSpawnSequence()
{
	const FVector PortalLocation = GetActorLocation();
	const FRotator PortalRotation = GetActorRotation();

	// 1. Play Audio
	if (PortalSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PortalSound, PortalLocation);
	}

	// 2. Spawn Niagara Portal Effect and store reference
	if (PortalVFX)
	{
		SpawnedPortalComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PortalVFX, PortalLocation, PortalRotation);
	}

	// 3. Immediately spawn companion actor behind portal and return reference to BP
	if (CompanionClass)
	{
		const FVector ForwardVector = GetActorForwardVector();
		const FVector SpawnLocation = PortalLocation - (ForwardVector * SpawnDistanceBehindPortal);
		const FRotator SpawnRotation = ForwardVector.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		SpawnedCompanion = GetWorld()->SpawnActor<ACompanionCharacter>(CompanionClass, SpawnLocation, SpawnRotation, SpawnParams);
	}

	// 4. Queue walkout logic after brief portal delay
	FTimerDelegate SpawnDelegate;
	SpawnDelegate.BindUObject(this, &ACompanionSpawner::Internal_SpawnAndWalk);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, SpawnDelegate, DelayBeforeWalkOut, false);

	return SpawnedCompanion;
}

void ACompanionSpawner::Internal_SpawnAndWalk()
{
	if (SpawnedCompanion)
	{
		AAIController* AIComp = Cast<AAIController>(SpawnedCompanion->GetController());
		if (!AIComp)
		{
			SpawnedCompanion->SpawnDefaultController();
			AIComp = Cast<AAIController>(SpawnedCompanion->GetController());
		}

		if (AIComp)
		{
			const FVector PortalLocation = GetActorLocation();
			const FVector ForwardVector = GetActorForwardVector();
			const FVector WalkToLocation = PortalLocation + (ForwardVector * WalkOutDistance);

			// Direct companion to walk out of the portal
			AIComp->MoveToLocation(WalkToLocation);
		}
	}

	// Stop spawning new particles so portal fades out naturally
	if (SpawnedPortalComponent)
	{
		SpawnedPortalComponent->Deactivate();
	}

	// Automatically destroy spawner actor after particles fade
	SetLifeSpan(3.0f);
}