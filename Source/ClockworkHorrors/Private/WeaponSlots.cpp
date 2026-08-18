// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "WeaponSlots.h"

#include "WeaponPickup.h"

#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"

UWeaponSlots::UWeaponSlots()
{
	PrimaryComponentTick.bCanEverTick = false;

	EquippedSlotNumber = 0;

	bSlotInputBindingsCreated = false;
}

void UWeaponSlots::BeginPlay()
{
	Super::BeginPlay();

	MaximumWeaponSlots = FMath::Clamp(MaximumWeaponSlots, 1, 9);

	StoredWeaponPickups.SetNum(MaximumWeaponSlots);

	SetupSlotInput();
}

void UWeaponSlots::SetupSlotInput()
{
	ACharacter* OwningCharacter = Cast<ACharacter>(GetOwner());

	if (!IsValid(OwningCharacter))
	{
		return;
	}

	APlayerController* OwningPlayerController = Cast<APlayerController>(OwningCharacter->GetController());

	if (!IsValid(OwningPlayerController))
	{
		return;
	}

	if (!OwningCharacter->InputComponent)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWeaponSlots::SetupSlotInput);

		return;
	}

	if (bSlotInputBindingsCreated)
	{
		return;
	}

	FInputKeyBinding& SlotOneBinding = OwningCharacter->InputComponent->BindKey(EKeys::One, IE_Pressed, this, &UWeaponSlots::HandleSlotOnePressed);
	SlotOneBinding.bConsumeInput = false;

	FInputKeyBinding& SlotTwoBinding = OwningCharacter->InputComponent->BindKey(EKeys::Two, IE_Pressed, this, &UWeaponSlots::HandleSlotTwoPressed);
	SlotTwoBinding.bConsumeInput = false;

	FInputKeyBinding& SlotThreeBinding = OwningCharacter->InputComponent->BindKey(EKeys::Three, IE_Pressed, this, &UWeaponSlots::HandleSlotThreePressed);
	SlotThreeBinding.bConsumeInput = false;

	FInputKeyBinding& SlotFourBinding = OwningCharacter->InputComponent->BindKey(EKeys::Four, IE_Pressed, this, &UWeaponSlots::HandleSlotFourPressed);
	SlotFourBinding.bConsumeInput = false;

	FInputKeyBinding& SlotFiveBinding = OwningCharacter->InputComponent->BindKey(EKeys::Five, IE_Pressed, this, &UWeaponSlots::HandleSlotFivePressed);
	SlotFiveBinding.bConsumeInput = false;

	FInputKeyBinding& SlotSixBinding = OwningCharacter->InputComponent->BindKey(EKeys::Six, IE_Pressed, this, &UWeaponSlots::HandleSlotSixPressed);
	SlotSixBinding.bConsumeInput = false;

	FInputKeyBinding& SlotSevenBinding = OwningCharacter->InputComponent->BindKey(EKeys::Seven, IE_Pressed, this, &UWeaponSlots::HandleSlotSevenPressed);
	SlotSevenBinding.bConsumeInput = false;

	FInputKeyBinding& SlotEightBinding = OwningCharacter->InputComponent->BindKey(EKeys::Eight, IE_Pressed, this, &UWeaponSlots::HandleSlotEightPressed);
	SlotEightBinding.bConsumeInput = false;

	FInputKeyBinding& SlotNineBinding = OwningCharacter->InputComponent->BindKey(EKeys::Nine, IE_Pressed, this, &UWeaponSlots::HandleSlotNinePressed);
	SlotNineBinding.bConsumeInput = false;

	FInputKeyBinding& DropWeaponBinding = OwningCharacter->InputComponent->BindKey(EKeys::F, IE_Pressed, this, &UWeaponSlots::HandleDropPressed);
	DropWeaponBinding.bConsumeInput = false;
	bSlotInputBindingsCreated = true;
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
	if (SlotNumber < 1 || SlotNumber > MaximumWeaponSlots)
	{
		return false;
	}

	UWeaponPickup* RequestedWeaponPickup = GetWeaponPickupInSlot(SlotNumber);

	if (!IsValid(RequestedWeaponPickup))
	{
		return false;
	}

	if (EquippedSlotNumber == SlotNumber && RequestedWeaponPickup->IsEquipped())
	{
		return true;
	}

	if (EquippedSlotNumber > 0)
	{
		UWeaponPickup* CurrentWeaponPickup = GetWeaponPickupInSlot(EquippedSlotNumber);

		if (IsValid(CurrentWeaponPickup) && CurrentWeaponPickup != RequestedWeaponPickup)
		{
			CurrentWeaponPickup->UnequipStoredWeapon();
		}
	}

	const bool bWeaponEquipped = RequestedWeaponPickup->EquipStoredWeapon();

	if (!bWeaponEquipped)
	{
		return false;
	}

	EquippedSlotNumber = SlotNumber;

	return true;
}

void UWeaponSlots::DropEquippedWeapon()
{
	if (EquippedSlotNumber <= 0)
	{
		return;
	}

	const int32 DroppedSlot = EquippedSlotNumber;

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

void UWeaponSlots::HandleSlotOnePressed()
{
	EquipSlot(1);
}

void UWeaponSlots::HandleSlotTwoPressed()
{
	EquipSlot(2);
}

void UWeaponSlots::HandleSlotThreePressed()
{
	EquipSlot(3);
}

void UWeaponSlots::HandleSlotFourPressed()
{
	EquipSlot(4);
}

void UWeaponSlots::HandleSlotFivePressed()
{
	EquipSlot(5);
}

void UWeaponSlots::HandleSlotSixPressed()
{
	EquipSlot(6);
}

void UWeaponSlots::HandleSlotSevenPressed()
{
	EquipSlot(7);
}

void UWeaponSlots::HandleSlotEightPressed()
{
	EquipSlot(8);
}

void UWeaponSlots::HandleSlotNinePressed()
{
	EquipSlot(9);
}

void UWeaponSlots::HandleDropPressed()
{
	DropEquippedWeapon();
}