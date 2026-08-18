// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/SpikeTrap.h"
#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"

ASpikeTrap::ASpikeTrap()
{
	// Tick is available, but starts disabled.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DamageBox->SetGenerateOverlapEvents(false);

	SpikeMeshes = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("SpikeMeshes"));

	SpikeMeshes->SetupAttachment(RootComponent);

	SpikeMeshes->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));

	TriggerBox->SetupAttachment(RootComponent);

	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	TriggerBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	TriggerBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TriggerBox->SetGenerateOverlapEvents(true);


	SpikeDamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpikeDamageBox"));

	SpikeDamageBox->SetupAttachment(SpikeMeshes);

	SpikeDamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SpikeDamageBox->SetCollisionResponseToAllChannels(ECR_Ignore);

	SpikeDamageBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	SpikeDamageBox->SetGenerateOverlapEvents(true);

	bDamagePlayer = true;
	bDamageOnEntry = true;
	bDamageOverTime = false;

	DamageAmount = 30.0f;
}

void ASpikeTrap::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	BuildSpikeLayout();

	UpdateCollisionSize();

	// Keep spikes visible while editing.
	SpikeMeshes->SetRelativeLocation(FVector::ZeroVector);
}

void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();

	BuildSpikeLayout();

	UpdateCollisionSize();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnTriggerBeginOverlap);

	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &ASpikeTrap::OnTriggerEndOverlap);

	SpikeDamageBox->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrap::OnSpikeDamageBeginOverlap);

	TrapState = ESpikeTrapState::Ready;

	CurrentSpikeOffset = -SpikeTravelDistance;

	// Start underground.
	SpikeMeshes->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentSpikeOffset));
}

void ASpikeTrap::BuildSpikeLayout()
{
	SpikeMeshes->ClearInstances();

	if (!SpikeMeshes->GetStaticMesh())
	{
		return;
	}

	const int32 SafeSpikesX = FMath::Max(1, SpikesX);

	const int32 SafeSpikesY = FMath::Max(1, SpikesY);

	const float TotalWidth = (SafeSpikesX - 1) * SpikeSpacing;

	const float TotalDepth = (SafeSpikesY - 1) * SpikeSpacing;

	const float StartX = -TotalWidth * 0.5f;

	const float StartY = -TotalDepth * 0.5f;

	for (int32 X = 0; X < SafeSpikesX; ++X)
	{
		for (int32 Y = 0; Y < SafeSpikesY; ++Y)
		{
			const FVector SpikeLocation(StartX + (X * SpikeSpacing), StartY + (Y * SpikeSpacing), 0.0f);

			const FTransform SpikeTransform(FRotator::ZeroRotator, SpikeLocation, FVector::OneVector);

			SpikeMeshes->AddInstance(SpikeTransform);
		}
	}
}

void ASpikeTrap::UpdateCollisionSize()
{
	const int32 SafeSpikesX = FMath::Max(1, SpikesX);

	const int32 SafeSpikesY = FMath::Max(1, SpikesY);

	const float TotalWidth = (SafeSpikesX - 1) * SpikeSpacing;

	const float TotalDepth = (SafeSpikesY - 1) * SpikeSpacing;

	const float HalfWidth = FMath::Max(SpikeSpacing * 0.5f, (TotalWidth * 0.5f) + (SpikeSpacing * 0.5f));

	const float HalfDepth = FMath::Max( SpikeSpacing * 0.5f, (TotalDepth * 0.5f) + (SpikeSpacing * 0.5f));

	// Trigger covers the spike area.
	TriggerBox->SetBoxExtent(FVector(HalfWidth, HalfDepth, 50.0f ) );

	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));

	float SpikeHalfHeight = 50.0f;

	if (SpikeMeshes->GetStaticMesh())
	{
		SpikeHalfHeight = SpikeMeshes ->GetStaticMesh() ->GetBounds().BoxExtent.Z;
	}

	SpikeDamageBox->SetBoxExtent(FVector(HalfWidth, HalfDepth, SpikeHalfHeight)
	);

	SpikeDamageBox->SetRelativeLocation(FVector(0.0f, 0.0f, SpikeHalfHeight));
}

void ASpikeTrap::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);

	if (!Player)
	{
		return;
	}

	PlayerInTrigger = Player;

	// Start the trap if it is currently ready.
	if (TrapState == ESpikeTrapState::Ready)
	{
		QueueActivation();
	}
}

void ASpikeTrap::OnTriggerEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);

	if (!Player)
	{
		return;
	}

	if (Player != PlayerInTrigger)
	{
		return;
	}

	PlayerInTrigger = nullptr;

	// If the spikes were only waiting to activate
	// and the player leaves, cancel that activation.
	if (TrapState == ESpikeTrapState::Waiting)
	{
		GetWorldTimerManager().ClearTimer(ActivationTimerHandle);

		TrapState = ESpikeTrapState::Ready;
	}
}

void ASpikeTrap::QueueActivation()
{
	if (!IsValid(PlayerInTrigger))
	{
		TrapState = ESpikeTrapState::Ready;

		return;
	}

	TrapState = ESpikeTrapState::Waiting;

	GetWorldTimerManager().SetTimer(ActivationTimerHandle, this, &ASpikeTrap::StartRising, ActivationDelay, false);
}

void ASpikeTrap::StartRising()
{
	// Don't activate if the player already left.
	if (!IsValid(PlayerInTrigger))
	{
		TrapState = ESpikeTrapState::Ready;

		return;
	}

	TrapState = ESpikeTrapState::Rising;

	SpikeDamageBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SetActorTickEnabled(true);
}

void ASpikeTrap::Tick(
	float DeltaTime
)
{
	Super::Tick(DeltaTime);


	if (TrapState == ESpikeTrapState::Rising)
	{
		CurrentSpikeOffset = FMath::FInterpConstantTo(CurrentSpikeOffset, 0.0f, DeltaTime, RiseSpeed);

		SpikeMeshes->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentSpikeOffset));

		if (FMath::IsNearlyEqual(CurrentSpikeOffset, 0.0f, 1.0f))
		{
			CurrentSpikeOffset = 0.0f;

			SpikeMeshes->SetRelativeLocation(FVector::ZeroVector);

			TrapState = ESpikeTrapState::Extended;

			SetActorTickEnabled(false);

			GetWorldTimerManager().SetTimer(ExtendedTimerHandle, this, &ASpikeTrap::StartRetracting, ExtendedTime, false);
		}

		return;
	}

	if (TrapState == ESpikeTrapState::Retracting)
	{
		CurrentSpikeOffset = FMath::FInterpConstantTo(CurrentSpikeOffset, -SpikeTravelDistance, DeltaTime,RetractSpeed);

		SpikeMeshes->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentSpikeOffset));

		if (FMath::IsNearlyEqual(CurrentSpikeOffset, -SpikeTravelDistance, 1.0f))
		{
			CurrentSpikeOffset = -SpikeTravelDistance;

			SpikeMeshes->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentSpikeOffset));

			SpikeDamageBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			SetActorTickEnabled(false);

			StartCooldown();
		}
	}
}

void ASpikeTrap::OnSpikeDamageBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (TrapState != ESpikeTrapState::Rising && TrapState != ESpikeTrapState::Extended)
	{
		return;
	}

	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);

	if (!Player)
	{
		return;
	}

	ApplyDamageToCharacter(Player);
}

void ASpikeTrap::StartRetracting()
{
	TrapState = ESpikeTrapState::Retracting;

	SetActorTickEnabled(true);
}

void ASpikeTrap::StartCooldown()
{
	TrapState = ESpikeTrapState::Cooldown;

	GetWorldTimerManager().SetTimer(CooldownTimerHandle, this, &ASpikeTrap::ResetTrap, CooldownTime,false);
}

void ASpikeTrap::ResetTrap()
{
	// If the player is STILL standing on the trap,
	// automatically start another cycle.
	if (IsValid(PlayerInTrigger))
	{
		QueueActivation();

		return;
	}

	TrapState =
		ESpikeTrapState::Ready;
}