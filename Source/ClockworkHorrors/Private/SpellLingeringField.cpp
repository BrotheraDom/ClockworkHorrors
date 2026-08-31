// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "SpellLingeringField.h"

#include "Spells.h"
#include "BaseCharacter.h"
#include "Enemy.h"
#include "Utils/HealthComponent.h"

#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

ASpellLingeringField::ASpellLingeringField()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(SceneRoot);
	SphereCollision->InitSphereRadius(250.0f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereCollision->SetCollisionObjectType(ECC_WorldDynamic);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(SceneRoot);
	BoxCollision->SetBoxExtent(FVector(250.0f, 250.0f, 50.0f));
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	BoxCollision->SetGenerateOverlapEvents(true);

	CastingActor = nullptr;
	SpellDefinition = nullptr;

	bShowLingeringHitbox = false;
}

void ASpellLingeringField::BeginPlay()
{
	Super::BeginPlay();
}

void ASpellLingeringField::InitializeLingeringField(AActor* InCastingActor, ASpells* InSpellDefinition)
{
	CastingActor = InCastingActor;
	SpellDefinition = InSpellDefinition;

	if (!SpellDefinition)
	{
		Destroy();
		return;
	}

	if (!SpellDefinition->ProducesLingeringField())
	{
		Destroy();
		return;
	}

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereCollision->SetRelativeLocation(FVector::ZeroVector);
	BoxCollision->SetRelativeLocation(FVector::ZeroVector);

	if (SpellDefinition->LingeringFieldShape == ELingeringFieldShape::Sphere)
	{
		SphereCollision->SetSphereRadius(SpellDefinition->LingeringFieldRadius);

		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereCollision->UpdateOverlaps();

		if (bShowLingeringHitbox)
		{
			DrawDebugSphere(
				GetWorld(),
				SphereCollision->GetComponentLocation(),
				SphereCollision->GetScaledSphereRadius(),
				32,
				FColor::Purple,
				false,
				SpellDefinition->LingeringFieldLifetimeSeconds,
				0,
				2.0f
			);
		}
	}
	else
	{
		const float HalfFieldSize = SpellDefinition->LingeringFieldSize * 0.5f;
		const float HalfFieldHeight = SpellDefinition->LingeringFieldHeight * 0.5f;

		BoxCollision->SetBoxExtent(
			FVector(
				HalfFieldSize,
				HalfFieldSize,
				HalfFieldHeight
			)
		);

		BoxCollision->SetRelativeLocation(
			FVector(
				0.0f,
				0.0f,
				HalfFieldHeight
			)
		);

		BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxCollision->UpdateOverlaps();

		if (bShowLingeringHitbox)
		{
			DrawDebugBox(
				GetWorld(),
				BoxCollision->GetComponentLocation(),
				BoxCollision->GetScaledBoxExtent(),
				BoxCollision->GetComponentQuat(),
				FColor::Purple,
				false,
				SpellDefinition->LingeringFieldLifetimeSeconds,
				0,
				2.0f
			);
		}
	}

	SetLifeSpan(SpellDefinition->LingeringFieldLifetimeSeconds);

	ApplyLingeringPulse();

	const float PulseInterval = FMath::Max(
		SpellDefinition->LingeringPulseIntervalSeconds,
		0.05f
	);

	GetWorldTimerManager().SetTimer(
		PulseTimer,
		this,
		&ASpellLingeringField::ApplyLingeringPulse,
		PulseInterval,
		true
	);
}

void ASpellLingeringField::ApplyLingeringPulse()
{
	if (!SpellDefinition)
	{
		return;
	}

	UPrimitiveComponent* ActiveFieldCollision = GetActiveFieldCollision();

	if (!IsValid(ActiveFieldCollision))
	{
		return;
	}

	TArray<AActor*> OverlappingActors;

	ActiveFieldCollision->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (!IsValid(OverlappingActor))
		{
			continue;
		}

		if (SpellDefinition->bLingeringFieldInflictsDamage)
		{
			if (AEnemy* EnemyCharacter = Cast<AEnemy>(OverlappingActor))
			{
				AController* DamageInstigator = nullptr;

				if (APawn* CastingPawn = Cast<APawn>(CastingActor))
				{
					DamageInstigator = CastingPawn->GetController();
				}

				UGameplayStatics::ApplyDamage(
					EnemyCharacter,
					SpellDefinition->LingeringDamagePerPulse,
					DamageInstigator,
					this,
					nullptr
				);
			}
		}

		if (SpellDefinition->bLingeringFieldRestoresHealth)
		{
			if (ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OverlappingActor))
			{
				UHealthComponent* HealthComponent = PlayerCharacter->FindComponentByClass<UHealthComponent>();

				if (HealthComponent && HealthComponent->CanHeal())
				{
					HealthComponent->Heal(
						SpellDefinition->LingeringHealthPerPulse
					);
				}
			}
		}
	}
}

UPrimitiveComponent* ASpellLingeringField::GetActiveFieldCollision() const
{
	if (!SpellDefinition)
	{
		return nullptr;
	}

	if (SpellDefinition->LingeringFieldShape == ELingeringFieldShape::Sphere)
	{
		return SphereCollision;
	}

	return BoxCollision;
}