// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/SpikeTrapPit.h"
#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASpikeTrapPit::ASpikeTrapPit()
{
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	KillVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("KillVolume"));
	KillVolume->SetupAttachment(Root);

	KillVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	KillVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	KillVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	KillVolume->SetGenerateOverlapEvents(true);

	KillVolume->SetBoxExtent(FVector(150.0f, 150.0f, 50.0f));

	KillDamage = 100.0f;
	bTrapEnabled = true;
}

// Called when the game starts or when spawned
void ASpikeTrapPit::BeginPlay()
{
	Super::BeginPlay();

	KillVolume->OnComponentBeginOverlap.AddDynamic(this, &ASpikeTrapPit::OnKillVolumeOverlap);
}

void ASpikeTrapPit::OnKillVolumeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bTrapEnabled || !OtherActor)
	{
		return;
	}

	ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor);

	if (!PlayerCharacter)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(PlayerCharacter, KillDamage, nullptr, this, UDamageType::StaticClass());
}