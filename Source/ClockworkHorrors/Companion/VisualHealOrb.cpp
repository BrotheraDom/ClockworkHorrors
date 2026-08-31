#include "VisualHealOrb.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Utils/HealthComponent.h"
#include "GameFramework/Actor.h"

AVisualHealOrb::AVisualHealOrb()
{
	PrimaryActorTick.bCanEverTick = true;

	OrbMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OrbMesh"));
	OrbMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = OrbMesh;
}

void AVisualHealOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// STATE 1: Moving from Orbit to Hand
	if (CurrentState == EOrbVisualState::MovingToHand && TargetMeshComponent)
	{
		const FVector TargetHandPos = TargetMeshComponent->GetSocketLocation(TargetSocketName);
		const FVector CurrentPos = GetActorLocation();

		const FVector NewPos = FMath::VInterpTo(CurrentPos, TargetHandPos, DeltaTime, GatherSpeed);
		SetActorLocation(NewPos);

		if (FVector::DistSquared(NewPos, TargetHandPos) < 25.0f) // Within 5cm
		{
			// Attach directly to hand
			const FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, false);
			AttachToComponent(TargetMeshComponent, AttachRules, TargetSocketName);
			CurrentState = EOrbVisualState::AttachedToHand;
		}
	}
	// STATE 2: Flying from Hand to Player Target
	else if (CurrentState == EOrbVisualState::FlyingToTarget && TargetPlayerActor)
	{
		// Target upper body / center of player
		const FVector TargetPos = TargetPlayerActor->GetActorLocation() + FVector(0.0f, 0.0f, 40.0f);
		const FVector CurrentPos = GetActorLocation();

		const FVector NewPos = FMath::VInterpTo(CurrentPos, TargetPos, DeltaTime, FlightSpeed);
		SetActorLocation(NewPos);

		if (FVector::DistSquared(NewPos, TargetPos) < 400.0f) // Within 20cm of player center
		{
			OnReachPlayer();
		}
	}
}

void AVisualHealOrb::MoveToHandSocket(USkeletalMeshComponent* MeshComp, FName SocketName)
{
	if (!MeshComp) return;

	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);

	TargetMeshComponent = MeshComp;
	TargetSocketName = SocketName;
	CurrentState = EOrbVisualState::MovingToHand;
}

void AVisualHealOrb::FlyToTargetActor(AActor* TargetActor, float HealAmount)
{
	if (!TargetActor) return;

	// 1. Store target data for Tick interpolation
	TargetPlayerActor = TargetActor;
	PendingHealAmount = HealAmount;
	CurrentState = EOrbVisualState::FlyingToTarget;

	// 2. Detach from hand mesh using explicit transform rules struct
	const FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);
}

void AVisualHealOrb::OnReachPlayer()
{
	CurrentState = EOrbVisualState::ReachedTarget;

	// Apply Healing directly when visual orb contacts player position
	if (TargetPlayerActor)
	{
		if (UHealthComponent* HealthComp = TargetPlayerActor->FindComponentByClass<UHealthComponent>())
		{
			HealthComp->Heal(PendingHealAmount);
		}
	}

	// Destroy visual orb after hit
	Destroy();
}