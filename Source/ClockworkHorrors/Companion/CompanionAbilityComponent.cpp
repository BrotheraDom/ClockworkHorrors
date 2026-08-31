#include "CompanionAbilityComponent.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Engine/World.h"
#include "Utils/HealthComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UCompanionAbilityComponent::UCompanionAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCompanionAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		LastFrameLocation = Owner->GetActorLocation();
	}

	SyncOrbActors();
}

void UCompanionAbilityComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	if (!Owner) return;

	FVector CurrentLocation = Owner->GetActorLocation();

	// 1. DISTANCE REGEN WHILE MOVING (Blocked while casting/holding an active orb)
	if (CurrentEssence < MaxEssence && !bIsResting && !ActiveCastingOrb && CurrentOrbState == EOrbFlightState::Orbiting)
	{
		float FrameDistance = FVector::Distance(CurrentLocation, LastFrameLocation);

		if (FrameDistance > 0.1f)
		{
			float DistanceEssenceToAdd = FrameDistance / DistancePerEssence;
			AddEssence(DistanceEssenceToAdd);
		}
	}

	LastFrameLocation = CurrentLocation;

	// 2. ORBIT POSITIONS
	if (ActiveOrbActors.Num() > 0)
	{
		CurrentOrbitAngle += OrbitSpeed * DeltaTime;
		if (CurrentOrbitAngle >= 360.0f)
		{
			CurrentOrbitAngle -= 360.0f;
		}
		UpdateOrbPositions();
	}

	// 3. CASTING FLIGHT
	if (ActiveCastingOrb && IsValid(ActiveCastingOrb))
	{
		ProcessCastingOrbFlight(DeltaTime);
	}
}

void UCompanionAbilityComponent::UpdateOrbPositions()
{
	const int32 TotalOrbs = ActiveOrbActors.Num();
	if (TotalOrbs == 0) return;

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner) return;

	FVector RelativeCenter = FVector(0.0f, 0.0f, OrbitHeightOffset);

	// Get world time or your accumulated DeltaTime variable for smooth continuous motion
	const float Time = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	const float BaseAngle = CurrentOrbitAngle; // Driven by your Tick rate

	for (int32 i = 0; i < TotalOrbs; ++i)
	{
		if (AActor* Orb = ActiveOrbActors[i])
		{
			if (!IsValid(Orb)) continue;

			// Determine group: even index = clockwise, odd index = counter-clockwise
			const bool bIsEven = (i % 2 == 0);
			const float Direction = bIsEven ? 1.0f : -1.0f;

			// Spread orbs evenly within their respective direction groups
			const float GroupIndex = FMath::FloorToFloat(i / 2.0f);
			const float GroupCount = FMath::CeilToFloat(TotalOrbs / 2.0f);
			const float AngleStep = 360.0f / FMath::Max(1.0f, GroupCount);

			// Calculate continuous rotation in opposite directions
			const float AngleDegrees = (BaseAngle * Direction) + (GroupIndex * AngleStep);
			const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);

			// 1. Base XY Orbit
			FVector LocalOffset = FVector(
				FMath::Cos(AngleRadians) * OrbitRadius,
				FMath::Sin(AngleRadians) * OrbitRadius,
				0.0f
			);

			// 2. Up & Down Wave Motion (Opposing phases for even/odd)
			// Adjust UpDownAmplitude (e.g. 40.0f - 60.0f) for more/less vertical height
			const float UpDownAmplitude = 50.0f;
			const float VerticalWaveSpeed = 2.5f;
			const float PhaseShift = bIsEven ? 0.0f : PI; // Invert wave for crisscross height

			LocalOffset.Z = FMath::Sin((Time * VerticalWaveSpeed) + PhaseShift + (GroupIndex * 1.2f)) * UpDownAmplitude;

			// 3. Opposing 3D Tilt Rotators (Tilts one group left-to-right, the other right-to-left)
			const float PitchAmount = bIsEven ? 30.0f : -30.0f;
			const float RollAmount = bIsEven ? 35.0f : -35.0f;
			FRotator OrbitTiltRotator = FRotator(PitchAmount, 0.0f, RollAmount);

			FVector TiltedOffset = OrbitTiltRotator.RotateVector(LocalOffset);

			// 4. Final Position Calculation
			FVector FinalRelativeLocation = RelativeCenter + TiltedOffset;

			// Safety floor clamp so orbs never clip terrain when sitting/resting
			const float MinRelativeZ = 20.0f;
			FinalRelativeLocation.Z = FMath::Max(FinalRelativeLocation.Z, MinRelativeZ);

			Orb->SetActorRelativeLocation(FinalRelativeLocation);
			Orb->SetActorRelativeRotation(FRotator::ZeroRotator);
		}
	}
}

void UCompanionAbilityComponent::AddEssence(float Amount)
{
	if (Amount <= 0.0f || CurrentEssence >= MaxEssence) return;

	EssenceAccumulator += Amount;

	int32 WholePointsGained = FMath::FloorToInt(EssenceAccumulator);
	if (WholePointsGained > 0)
	{
		EssenceAccumulator -= WholePointsGained;
		CurrentEssence = FMath::Clamp(CurrentEssence + WholePointsGained, 0, MaxEssence);

		OnEssenceChanged.Broadcast(CurrentEssence, MaxEssence);
		SyncOrbActors();

		if (AAIController* AIComp = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController()))
		{
			if (UBlackboardComponent* BB = AIComp->GetBlackboardComponent())
			{
				BB->SetValueAsInt(FName("EssenceCount"), CurrentEssence);
			}
		}
	}
}

bool UCompanionAbilityComponent::ConsumeEssence(int32 Amount)
{
	if (Amount <= 0 || CurrentEssence < Amount) return false;

	CurrentEssence -= Amount;

	OnEssenceChanged.Broadcast(CurrentEssence, MaxEssence);

	if (AAIController* AIComp = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController()))
	{
		if (UBlackboardComponent* BB = AIComp->GetBlackboardComponent())
		{
			BB->SetValueAsInt(FName("EssenceCount"), CurrentEssence);
		}
	}

	return true;
}

void UCompanionAbilityComponent::SyncOrbActors()
{
	UWorld* World = GetWorld();
	AActor* Owner = GetOwner();
	if (!World || !Owner || !OrbActorClass) return;

	while (ActiveOrbActors.Num() < CurrentEssence)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Owner;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* NewOrb = World->SpawnActor<AActor>(OrbActorClass, Owner->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
		if (NewOrb)
		{
			NewOrb->SetActorEnableCollision(false);
			NewOrb->SetActorScale3D(OrbScale);

			// Snap origin to Owner root so relative math orbits around the character center
			NewOrb->AttachToActor(Owner, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

			ActiveOrbActors.Add(NewOrb);
		}
	}

	while (ActiveOrbActors.Num() > CurrentEssence)
	{
		AActor* OrbToDestroy = ActiveOrbActors.Pop();
		if (IsValid(OrbToDestroy))
		{
			OrbToDestroy->Destroy();
		}
	}

	UpdateOrbPositions();
}

void UCompanionAbilityComponent::Notify_SpawnHandOrb()
{
	// Guard checks
	if (CurrentEssence < HealingTouchCost || ActiveOrbActors.Num() == 0) return;

	// 1. Pop the orb directly (This takes 1 orb out of orbit visually and physically)
	ActiveCastingOrb = ActiveOrbActors.Pop();
	if (!ActiveCastingOrb) return;

	// 2. Reduce Essence integer directly WITHOUT triggering SyncOrbActors
	CurrentEssence = FMath::Clamp(CurrentEssence - HealingTouchCost, 0, MaxEssence);

	// 3. Broadcast to UI and update Blackboard key
	OnEssenceChanged.Broadcast(CurrentEssence, MaxEssence);

	if (AAIController* AIComp = Cast<AAIController>(Cast<APawn>(GetOwner())->GetController()))
	{
		if (UBlackboardComponent* BB = AIComp->GetBlackboardComponent())
		{
			BB->SetValueAsInt(FName("EssenceCount"), CurrentEssence);
		}
	}

	// 4. Attach to hand
	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner || !CharacterOwner->GetMesh()) return;

	const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
	ActiveCastingOrb->AttachToComponent(CharacterOwner->GetMesh(), AttachRules, FName("hand_r"));
	ActiveCastingOrb->SetActorScale3D(OrbScale);

	CurrentOrbState = EOrbFlightState::AttachedToHand;
}

void UCompanionAbilityComponent::Notify_LaunchHandOrb(AActor* TargetPlayer)
{
	AActor* Target = TargetPlayer ? TargetPlayer : CurrentTargetPlayer;
	if (!ActiveCastingOrb || !Target) return;

	const FDetachmentTransformRules DetachRules(
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		EDetachmentRule::KeepWorld,
		false
	);
	ActiveCastingOrb->DetachFromActor(DetachRules);

	CurrentTargetPlayer = Target;
	CurrentOrbState = EOrbFlightState::FlyingToTarget;
}

void UCompanionAbilityComponent::ProcessCastingOrbFlight(float DeltaTime)
{
	if (CurrentOrbState != EOrbFlightState::FlyingToTarget) return;

	// Crash/Stuck Guard
	if (!IsValid(ActiveCastingOrb) || !IsValid(CurrentTargetPlayer))
	{
		CurrentOrbState = EOrbFlightState::Orbiting;
		ActiveCastingOrb = nullptr;
		CurrentTargetPlayer = nullptr;
		return;
	}

	const FVector TargetPos = CurrentTargetPlayer->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
	const FVector CurrentPos = ActiveCastingOrb->GetActorLocation();

	const FVector NewPos = FMath::VInterpConstantTo(CurrentPos, TargetPos, DeltaTime, FlightSpeed);
	ActiveCastingOrb->SetActorLocation(NewPos);

	const float DistanceToTarget = FVector::Distance(NewPos, TargetPos);
	const float DistanceToPlayerActor = FVector::Distance(NewPos, CurrentTargetPlayer->GetActorLocation());

	if (DistanceToTarget <= 50.0f || DistanceToPlayerActor <= 60.0f)
	{
		if (UHealthComponent* HealthComp = CurrentTargetPlayer->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->Heal(HealingTouchAmount);
		}

		ActiveCastingOrb->Destroy();
		ActiveCastingOrb = nullptr;
		CurrentTargetPlayer = nullptr;

		// Cleanly reset state so the BT task finishes
		CurrentOrbState = EOrbFlightState::Orbiting;
	}
}

bool UCompanionAbilityComponent::PerformHealingTouch(AActor* TargetActor)
{
	if (CurrentEssence < HealingTouchCost) return false;

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());
	if (!CharacterOwner || !CastHealMontage) return false;

	USkeletalMeshComponent* Mesh = CharacterOwner->GetMesh();
	if (!Mesh) return false;

	UAnimInstance* AnimInst = Mesh->GetAnimInstance();
	if (!AnimInst) return false;

	CurrentTargetPlayer = IsValid(TargetActor) ? TargetActor : UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (!CurrentTargetPlayer) return false;

	AnimInst->Montage_Play(CastHealMontage, 1.0f);
	return true;
}