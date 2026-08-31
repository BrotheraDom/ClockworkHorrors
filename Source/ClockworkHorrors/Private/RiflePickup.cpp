#include "RiflePickup.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include <BaseCharacter.h>

ARiflePickup::ARiflePickup()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	RifleMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RifleMesh"));
	RifleMesh->SetupAttachment(SceneRoot);
	RifleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupRange = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRange"));
	PickupRange->SetupAttachment(SceneRoot);
	PickupRange->SetSphereRadius(150.0f);
	PickupRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	PickupRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	PickupRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	PickupRange->OnComponentBeginOverlap.AddDynamic(this, &ARiflePickup::BoundOverlap);
	PickupRange->OnComponentEndOverlap.AddDynamic(this, &ARiflePickup::LeaveOverlap);
}

void ARiflePickup::BoundOverlap(UPrimitiveComponent* OverlappedComponenet, AActor* actor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ABaseCharacter>(actor))
	{
		playerInRange = true;
	}
}

void ARiflePickup::LeaveOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ABaseCharacter>(OtherActor))
	{
	playerInRange = false;
	}

}