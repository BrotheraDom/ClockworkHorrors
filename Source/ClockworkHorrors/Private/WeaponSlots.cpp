// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "WeaponSlots.h"

#include "WeaponPickup.h"

#include "GameFramework/Character.h"

UWeaponSlots::UWeaponSlots()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquippedSlotNumber = 0;
}

void UWeaponSlots::BeginPlay()
{
	Super::BeginPlay();

	MaximumWeaponSlots = FMath::Clamp(MaximumWeaponSlots, 1, 9);

	StoredWeaponPickups.SetNum(MaximumWeaponSlots);
}

bool UWeaponSlots::AddWeapon(UWeaponPickup* NewWeaponPickup)
{
	if (!IsValid(NewWeaponPickup))
	{
		return false;
	}

	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());

	if (!IsValid(OwningCharacter))
	{
		return false;
	}

	const int32 RequestedSlot = NewWeaponPickup->GetInventorySlot();

	if (RequestedSlot < 1 || RequestedSlot > MaximumWeaponSlots)
	{
		return false;
	}

	const int32 WeaponArrayIndex = RequestedSlot - 1;

	if (!StoredWeaponPickups.IsValidIndex(WeaponArrayIndex))
	{
		return false;
	}

	UWeaponPickup* ExistingWeaponPickup = StoredWeaponPickups[WeaponArrayIndex];

	if (IsValid(ExistingWeaponPickup))
	{
		if (ExistingWeaponPickup == NewWeaponPickup)
		{
			return EquipSlot(RequestedSlot);
		}

		return false;
	}

	if (NewWeaponPickup->IsStored() && NewWeaponPickup->GetStoredHolder() != OwningCharacter)
	{
		return false;
	}

	if (!NewWeaponPickup->IsStored())
	{
		const bool bWeaponStored = NewWeaponPickup->StoreWeapon(OwningCharacter);

		if (!bWeaponStored)
		{
			return false;
		}
	}

	StoredWeaponPickups[WeaponArrayIndex] = NewWeaponPickup;

	return EquipSlot(RequestedSlot);
}

bool UWeaponSlots::EquipSlot(int32 SlotNumber)
{
	UE_LOG(
		LogTemp,
		Warning,
		TEXT(
			"WeaponSlots::EquipSlot requested %d. Equipped=%d. Maximum=%d"
		),
		SlotNumber,
		EquippedSlotNumber,
		MaximumWeaponSlots
	);

	if (SlotNumber < 1 || SlotNumber > MaximumWeaponSlots)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon slot number is out of range."));
		return false;
	}

	UWeaponPickup* RequestedWeaponPickup =
		GetWeaponPickupInSlot(SlotNumber);

	if (!IsValid(RequestedWeaponPickup))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No registered weapon in weapon slot %d."),
			SlotNumber
		);
		return false;
	}

	if (EquippedSlotNumber == SlotNumber &&
		RequestedWeaponPickup->IsEquipped())
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon slot %d already equipped."), SlotNumber);
		return true;
	}

	if (EquippedSlotNumber > 0)
	{
		UWeaponPickup* CurrentWeaponPickup =
			GetWeaponPickupInSlot(EquippedSlotNumber);

		if (IsValid(CurrentWeaponPickup) &&
			CurrentWeaponPickup != RequestedWeaponPickup)
		{
			CurrentWeaponPickup->UnequipStoredWeapon();
		}
	}

	const bool bWeaponEquipped =
		RequestedWeaponPickup->EquipStoredWeapon();

	if (!bWeaponEquipped)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("EquipStoredWeapon failed for weapon slot %d."),
			SlotNumber
		);
		return false;
	}

	EquippedSlotNumber = SlotNumber;

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Weapon slot %d equipped successfully."),
		SlotNumber
	);

	return true;
}

void UWeaponSlots::DropEquippedWeapon(int slot)
{
	const int32 DroppedSlot = slot;
	const int32 WeaponArrayIndex = DroppedSlot - 1;

	if (!StoredWeaponPickups.IsValidIndex(WeaponArrayIndex))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("WeaponArrayIndex is invalid."));
		return;
	}

	UWeaponPickup* EquippedWeaponPickup = GetWeaponPickupInSlot(DroppedSlot);

	if (!IsValid(EquippedWeaponPickup))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("EquippedWeaponPickup is invalid."));
		return;
	}

	StoredWeaponPickups[WeaponArrayIndex] = nullptr;

	EquippedWeaponPickup->DropWeapon();
}

// Old Code for DropEquippedWeapon, kept for reference
/*
void UWeaponSlots::DropEquippedWeapon()
{
	if (EquippedSlotNumber <= 0)
	{
		return;
	}

	const int32 DroppedSlot = EquippedSlotNumber;
	const int32 WeaponArrayIndex = DroppedSlot - 1;

	UWeaponPickup* EquippedWeaponPickup = GetWeaponPickupInSlot(DroppedSlot);

	if (!IsValid(EquippedWeaponPickup))
	{
		EquippedSlotNumber = 0;
		return;
	}

	const int32 WeaponArrayIndex = DroppedSlot - 1;

	if (StoredWeaponPickups.IsValidIndex(WeaponArrayIndex))
	{
		StoredWeaponPickups[WeaponArrayIndex] = nullptr;
	}

	EquippedSlotNumber = 0;

	EquippedWeaponPickup->DropWeapon();
}
*/

bool UWeaponSlots::HasWeaponInSlot(int32 SlotNumber) const
{
	return IsValid(GetWeaponPickupInSlot(SlotNumber));
}

AActor* UWeaponSlots::GetWeaponInSlot(int32 SlotNumber) const
{
	UWeaponPickup* WeaponPickup = GetWeaponPickupInSlot(SlotNumber);

	if (!IsValid(WeaponPickup))
	{
		return nullptr;
	}

	return WeaponPickup->GetOwner();
}

AActor* UWeaponSlots::GetEquippedWeapon() const
{
	if (EquippedSlotNumber <= 0)
	{
		return nullptr;
	}

	return GetWeaponInSlot(EquippedSlotNumber);
}

int32 UWeaponSlots::GetEquippedSlot() const
{
	return EquippedSlotNumber;
}

UWeaponPickup* UWeaponSlots::GetWeaponPickupInSlot(int32 SlotNumber) const
{
	if (SlotNumber < 1 || SlotNumber > MaximumWeaponSlots)
	{
		return nullptr;
	}

	const int32 WeaponArrayIndex = SlotNumber - 1;

	if (!StoredWeaponPickups.IsValidIndex(WeaponArrayIndex))
	{
		return nullptr;
	}

	return StoredWeaponPickups[WeaponArrayIndex];
}