// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "BaseSword.h"
#include "BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "Engine/DamageEvents.h"


ABaseSword::ABaseSword()
{
	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SetRootComponent(staticMesh);
	staticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HitBox = CreateDefaultSubobject<UBoxComponent>("HitBox");
	HitBox->SetupAttachment(GetRootComponent());
	HitBox->SetRelativeLocation(FVector(0, 0, 65));
	HitBox->SetRelativeScale3D(FVector(0.7925, 0.7925, 0.7925));
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	HitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	HitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	HitBox->SetBoxExtent(FVector(5.811434, 11.917904, 50.987691));
}

void ABaseSword::BeginPlay()
{
	Super::BeginPlay();
	HitBox->OnComponentBeginOverlap.AddDynamic(this, &ABaseSword::BoundOverlap);
}

void ABaseSword::Attack()
{
	ABaseWeapon::Attack();
	if (CanAttack())
	{
		ActionHappening = true;
		ResetSwordHit();
		ABaseCharacter* character = Cast<ABaseCharacter>(ParentPawn);
		if (character)
		{
			character->OnAttack.Broadcast();
		}
	}
}

void ABaseSword::BoundOverlap(UPrimitiveComponent* OverlappedComponenet, AActor* actor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<AEnemy>(actor))
	{
		TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
		FDamageEvent DamageEvent(ValidDamageTypeClass);
		ABaseCharacter* character = Cast<ABaseCharacter>(ParentPawn);
		if (actor->GetInstigatorController() != character->GetController())
		{
			actor->TakeDamage(damage, DamageEvent, character->GetController(), this);
			HitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void ABaseSword::ResetSwordHit()
{
	HitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
