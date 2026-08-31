// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "WeaponPickup.h"

#include "WeaponSlots.h"

#include "Components/InputComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"

UWeaponPickup::UWeaponPickup()
{
	PrimaryComponentTick.bCanEverTick = false;

	PickupVolume = nullptr;
	WeaponPhysicsComponent = nullptr;
	NearbyPlayer = nullptr;
	WeaponHolder = nullptr;

	bWeaponEquipped = false;
}

void UWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	FindWeaponPhysicsComponent();
	CreatePickupVolume();

	if (bBeginWithPhysics && IsValid(WeaponPhysicsComponent))
	{
		EnableWorldWeaponState();
	}
	NearbyPlayer = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UWeaponPickup::FindWeaponPhysicsComponent()
{
	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	WeaponPhysicsComponent = Cast<UPrimitiveComponent>(WeaponOwner->GetRootComponent());

	if (IsValid(WeaponPhysicsComponent))
	{
		return;
	}

	TArray<UPrimitiveComponent*> PrimitiveComponents;

	WeaponOwner->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

	if (PrimitiveComponents.Num() > 0)
	{
		WeaponPhysicsComponent = PrimitiveComponents[0];
	}
}

void UWeaponPickup::CreatePickupVolume()
{
	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	if (!WeaponOwner->GetRootComponent())
	{
		return;
	}

	PickupVolume = NewObject<USphereComponent>(WeaponOwner, TEXT("WeaponPickupVolume"));

	if (!IsValid(PickupVolume))
	{
		return;
	}

	WeaponOwner->AddInstanceComponent(PickupVolume);

	PickupVolume->SetupAttachment(WeaponOwner->GetRootComponent());

	PickupVolume->InitSphereRadius(PickupRadius);

	PickupVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	PickupVolume->SetCollisionObjectType(ECC_WorldDynamic);

	PickupVolume->SetCollisionResponseToAllChannels(ECR_Ignore);

	PickupVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	PickupVolume->SetGenerateOverlapEvents(true);

	PickupVolume->RegisterComponent();

	PickupVolume->OnComponentBeginOverlap.AddDynamic(this, &UWeaponPickup::HandlePickupVolumeEntered);

	PickupVolume->OnComponentEndOverlap.AddDynamic(this, &UWeaponPickup::HandlePickupVolumeExited);
}



void UWeaponPickup::HandlePickupVolumeEntered(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsStored())
	{
		return;
	}

	ACharacter* CharacterEnteringRange = Cast<ACharacter>(OtherActor);

	if (!IsValid(CharacterEnteringRange))
	{
		return;
	}

	APlayerController* PlayerController = Cast<APlayerController>(CharacterEnteringRange->GetController());

	if (!IsValid(PlayerController))
	{
		return;
	}

	NearbyPlayer = CharacterEnteringRange;
}

void UWeaponPickup::HandlePickupVolumeExited(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	ACharacter* CharacterLeavingRange = Cast<ACharacter>(OtherActor);

	if (!IsValid(CharacterLeavingRange))
	{
		return;
	}

	if (NearbyPlayer != CharacterLeavingRange)
	{
		return;
	}

	NearbyPlayer = nullptr;
}

void UWeaponPickup::HandleInteractPressed()
{
	if (IsStored())
	{
		return;
	}

	if (!IsValid(NearbyPlayer))
	{
		return;
	}

	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	const float DistanceToWeapon = FVector::Dist(WeaponOwner->GetActorLocation(), NearbyPlayer->GetActorLocation());

	if (DistanceToWeapon > PickupRadius + 100.0f)
	{
		NearbyPlayer = nullptr;
		return;
	}

	PickupWeapon(NearbyPlayer);
}

bool UWeaponPickup::PickupWeapon(ACharacter* NewHolder)
{
	if (!IsValid(NewHolder))
	{
		return false;
	}

	if (IsStored())
	{
		return false;
	}

	UWeaponSlots* WeaponSlots = NewHolder->FindComponentByClass<UWeaponSlots>();

	if (!IsValid(WeaponSlots))
	{
		return false;
	}

	return WeaponSlots->AddWeapon(this);
}

bool UWeaponPickup::StoreWeapon(ACharacter* NewHolder)
{
	if (!IsValid(NewHolder))
	{
		return false;
	}

	if (IsValid(WeaponHolder) && WeaponHolder != NewHolder)
	{
		return false;
	}

	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return false;
	}

	if (!NewHolder->GetMesh())
	{
		return false;
	}

	DisableWorldWeaponState();

	const bool bWeaponAttached = WeaponOwner->AttachToComponent(NewHolder->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GripSocketName);

	if (!bWeaponAttached)
	{
		EnableWorldWeaponState();
		return false;
	}

	WeaponHolder = NewHolder;

	bWeaponEquipped = false;

	NearbyPlayer = nullptr;

	WeaponOwner->SetActorRelativeLocation(EquippedRelativeLocation);

	WeaponOwner->SetActorRelativeRotation(EquippedRelativeRotation);

	WeaponOwner->SetActorRelativeScale3D(EquippedRelativeScale);

	WeaponOwner->SetActorHiddenInGame(true);

	return true;
}

bool UWeaponPickup::EquipStoredWeapon()
{
	if (!IsValid(WeaponHolder))
	{
		return false;
	}

	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return false;
	}

	if (bWeaponEquipped)
	{
		return true;
	}

	bWeaponEquipped = true;

	WeaponOwner->SetActorHiddenInGame(false);

	OnWeaponEquippedStateChanged.Broadcast(true);

	return true;
}

void UWeaponPickup::UnequipStoredWeapon()
{
	if (!IsValid(WeaponHolder))
	{
		return;
	}

	if (!bWeaponEquipped)
	{
		return;
	}

	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	bWeaponEquipped = false;

	WeaponOwner->SetActorHiddenInGame(true);

	OnWeaponEquippedStateChanged.Broadcast(false);
}

void UWeaponPickup::DropWeapon()
{
	if (!IsValid(WeaponHolder))
	{
		return;
	}

	AActor* WeaponOwner = GetOwner();

	if (!IsValid(WeaponOwner))
	{
		return;
	}

	ACharacter* PreviousHolder = WeaponHolder;

	const bool bWasEquipped = bWeaponEquipped;

	const FVector WeaponDropLocation = PreviousHolder->GetActorLocation() + PreviousHolder->GetActorForwardVector() * DropForwardDistance + FVector(0.0f, 0.0f, DropHeight);

	WeaponOwner->SetActorHiddenInGame(false);

	WeaponOwner->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	WeaponOwner->SetActorLocation(WeaponDropLocation, false, nullptr, ETeleportType::TeleportPhysics);

	WeaponHolder = nullptr;

	bWeaponEquipped = false;

	NearbyPlayer = PreviousHolder;

	EnableWorldWeaponState();

	if (bWasEquipped)
	{
		OnWeaponEquippedStateChanged.Broadcast(false);
	}
}

void UWeaponPickup::DisableWorldWeaponState()
{
	if (IsValid(WeaponPhysicsComponent))
	{
		WeaponPhysicsComponent->SetSimulatePhysics(false);

		WeaponPhysicsComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);

		WeaponPhysicsComponent->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		WeaponPhysicsComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (IsValid(PickupVolume))
	{
		PickupVolume->SetGenerateOverlapEvents(false);

		PickupVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void UWeaponPickup::EnableWorldWeaponState()
{
	if (IsValid(WeaponPhysicsComponent))
	{
		WeaponPhysicsComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponPhysicsComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
		WeaponPhysicsComponent->SetCollisionResponseToAllChannels(ECR_Block);
		WeaponPhysicsComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		WeaponPhysicsComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		WeaponPhysicsComponent->SetSimulatePhysics(true);
		WeaponPhysicsComponent->WakeAllRigidBodies();
	}

	if (IsValid(PickupVolume))
	{
		PickupVolume->SetGenerateOverlapEvents(true);

		PickupVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
}

bool UWeaponPickup::GripSocketAlreadyOccupied(ACharacter* ProspectiveHolder) const
{
	if (!IsValid(ProspectiveHolder))
	{
		return false;
	}

	AActor* WeaponOwner = GetOwner();

	TArray<AActor*> AttachedActors;

	ProspectiveHolder->GetAttachedActors(AttachedActors, true, false);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!IsValid(AttachedActor))
		{
			continue;
		}

		if (AttachedActor == WeaponOwner)
		{
			continue;
		}

		if (AttachedActor->GetAttachParentActor() != ProspectiveHolder)
		{
			continue;
		}

		if (AttachedActor->GetAttachParentSocketName() == GripSocketName)
		{
			return true;
		}
	}

	return false;
}

bool UWeaponPickup::IsEquipped() const
{
	return IsValid(WeaponHolder) && bWeaponEquipped;
}

bool UWeaponPickup::IsStored() const
{
	return IsValid(WeaponHolder);
}

ACharacter* UWeaponPickup::GetEquippedHolder() const
{
	if (!IsEquipped())
	{
		return nullptr;
	}

	return WeaponHolder;
}

ACharacter* UWeaponPickup::GetStoredHolder() const
{
	return WeaponHolder;
}

int32 UWeaponPickup::GetInventorySlot() const
{
	return InventorySlot;
}
