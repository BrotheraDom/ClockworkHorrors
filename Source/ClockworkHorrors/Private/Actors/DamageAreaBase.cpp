// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/DamageAreaBase.h"
#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ADamageAreaBase::ADamageAreaBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DamageBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DamageBox"));

	SetRootComponent(DamageBox);

	DamageBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageBox->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap);
	DamageBox->SetGenerateOverlapEvents(true);
}

// Called when the game starts or when spawned
void ADamageAreaBase::BeginPlay()
{
	Super::BeginPlay();
	DamageBox->OnComponentBeginOverlap.AddDynamic(this, &ADamageAreaBase::OnDamageBoxBeginOverlap);

	DamageBox->OnComponentEndOverlap.AddDynamic(this, &ADamageAreaBase::OnDamageBoxEndOverlap);
}

void ADamageAreaBase::OnDamageBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!bDamagePlayer)
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);

	if (!Player)
	{
		return;
	}

	PlayerInDamageArea = Player;

	if (bDamageOnEntry)
	{
		DamagePlayer();
	}

	if (bDamageOverTime && DamageInterval > 0.0f)
	{
		if (!GetWorldTimerManager().IsTimerActive(DamageTimerHandle))
		{
			GetWorldTimerManager().SetTimer(
				DamageTimerHandle,
				this,
				&ADamageAreaBase::DamagePlayer,
				DamageInterval,
				true
			);
		}
	}
}

void ADamageAreaBase::OnDamageBoxEndOverlap( UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!PlayerInDamageArea)
	{
		return;
	}

	if (OtherActor != PlayerInDamageArea)
	{
		return;
	}

	PlayerInDamageArea = nullptr;

	GetWorldTimerManager().ClearTimer(DamageTimerHandle);
}

void ADamageAreaBase::DamagePlayer()
{
	if (!bDamagePlayer)
	{
		GetWorldTimerManager().ClearTimer(DamageTimerHandle);
		return;
	}

	if (!IsValid(PlayerInDamageArea))
	{
		PlayerInDamageArea = nullptr;

		GetWorldTimerManager().ClearTimer(DamageTimerHandle);

		return;
	}

	if (DamageAmount <= 0.0f)
	{
		return;
	}

	ApplyDamageToCharacter(PlayerInDamageArea);
}

void ADamageAreaBase::ApplyDamageToCharacter(ABaseCharacter* Character)
{
	if (!bDamagePlayer || !IsValid(Character) || DamageAmount <= 0.0f)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(
		Character,
		DamageAmount,
		nullptr,
		this,
		UDamageType::StaticClass()
	);
}

