// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Staff.h"

#include "SpellProjectile.h"
#include "WeaponPickup.h"

#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"

AStaff::AStaff()
{
	PrimaryActorTick.bCanEverTick = false;

	StaffAppearance = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaffAppearance"));

	SetRootComponent(StaffAppearance);

	StaffAppearance->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	StaffAppearance->SetCollisionObjectType(ECC_WorldDynamic);

	StaffAppearance->SetCollisionResponseToAllChannels(ECR_Block);

	StaffAppearance->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	StaffAppearance->SetSimulatePhysics(false);

	SpellReleasePoint = CreateDefaultSubobject<USceneComponent>(TEXT("SpellReleasePoint"));

	SpellReleasePoint->SetupAttachment(StaffAppearance);

	SpellSlots.SetNum(4);

	WeaponPickup = nullptr;

	bSpellReady = true;
	bStaffInputBindingsCreated = false;

	SelectedSpellIndex = 0;
}

void AStaff::BeginPlay()
{
	Super::BeginPlay();

	if (SpellSlots.Num() != 4)
	{
		SpellSlots.SetNum(4);
	}

	WeaponPickup = FindComponentByClass<UWeaponPickup>();

	if (IsValid(WeaponPickup))
	{
		WeaponPickup->OnWeaponEquippedStateChanged.AddDynamic(this, &AStaff::HandleWeaponEquippedStateChanged);
	}

	if (!SpellSlots.IsValidIndex(SelectedSpellIndex) || !SpellSlots[SelectedSpellIndex])
	{
		for (int32 SpellSlotIndex = 0; SpellSlotIndex < SpellSlots.Num(); SpellSlotIndex++)
		{
			if (SpellSlots[SpellSlotIndex])
			{
				SelectedSpellIndex = SpellSlotIndex;
				break;
			}
		}
	}

	SetupStaffInput();
}

void AStaff::SetupStaffInput()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	if (!IsValid(PlayerController))
	{
		return;
	}

	EnableInput(PlayerController);

	if (!InputComponent)
	{
		return;
	}

	InputComponent->Priority = 100;

	if (bStaffInputBindingsCreated)
	{
		UpdateStaffInputConsumption();
		return;
	}

	FInputKeyBinding& StaffCastBinding = InputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AStaff::HandleCastPressed);

	StaffCastBinding.bConsumeInput = false;

	FInputKeyBinding& StaffScrollUpBinding = InputComponent->BindKey(EKeys::MouseScrollUp, IE_Pressed, this, &AStaff::HandleSpellScrollUp);

	StaffScrollUpBinding.bConsumeInput = false;

	FInputKeyBinding& StaffScrollDownBinding = InputComponent->BindKey(EKeys::MouseScrollDown, IE_Pressed, this, &AStaff::HandleSpellScrollDown);

	StaffScrollDownBinding.bConsumeInput = false;

	bStaffInputBindingsCreated = true;

	UpdateStaffInputConsumption();
}

bool AStaff::IsStaffControlledByPlayer() const
{
	if (!IsValid(WeaponPickup))
	{
		return false;
	}

	if (!WeaponPickup->IsEquipped())
	{
		return false;
	}

	ACharacter* StaffHolder = WeaponPickup->GetEquippedHolder();

	if (!IsValid(StaffHolder))
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(StaffHolder->GetController());

	return IsValid(PlayerController);
}

void AStaff::HandleWeaponEquippedStateChanged(bool bWeaponEquipped)
{
	UpdateStaffInputConsumption();
}

void AStaff::UpdateStaffInputConsumption()
{
	if (!InputComponent)
	{
		return;
	}

	const bool bShouldConsumeStaffInput = IsStaffControlledByPlayer();

	for (FInputKeyBinding& InputBinding : InputComponent->KeyBindings)
	{
		if (InputBinding.Chord.Key == EKeys::LeftMouseButton && InputBinding.KeyEvent == IE_Pressed)
		{
			InputBinding.bConsumeInput = bShouldConsumeStaffInput;
		}

		if ((InputBinding.Chord.Key == EKeys::MouseScrollUp || InputBinding.Chord.Key == EKeys::MouseScrollDown) && InputBinding.KeyEvent == IE_Pressed)
		{
			InputBinding.bConsumeInput = bShouldConsumeStaffInput;
		}
	}
}

void AStaff::HandleCastPressed()
{
	if (!IsStaffControlledByPlayer())
	{
		return;
	}

	ACharacter* StaffHolder = WeaponPickup->GetEquippedHolder();

	if (!IsValid(StaffHolder))
	{
		return;
	}

	FVector CastingDirection = StaffHolder->GetActorForwardVector();

	APlayerController* PlayerController = Cast<APlayerController>(StaffHolder->GetController());

	if (IsValid(PlayerController))
	{
		CastingDirection = PlayerController->GetControlRotation().Vector();
	}

	AttemptSpellCast(StaffHolder, CastingDirection);
}

void AStaff::HandleSpellScrollUp()
{
	if (!IsStaffControlledByPlayer())
	{
		return;
	}

	SelectPreviousSpell();
}

void AStaff::HandleSpellScrollDown()
{
	if (!IsStaffControlledByPlayer())
	{
		return;
	}

	SelectNextSpell();
}

bool AStaff::AttemptSpellCast(AActor* CastingActor, FVector CastingDirection)
{
	if (!IsValid(CastingActor))
	{
		return false;
	}

	if (!bSpellReady)
	{
		return false;
	}

	ASpells* SelectedSpell = GetSelectedSpell();

	if (!IsValid(SelectedSpell))
	{
		return false;
	}

	if (CastingDirection.IsNearlyZero())
	{
		CastingDirection = CastingActor->GetActorForwardVector();
	}

	CastingDirection.Normalize();

	bSpellReady = false;

	SpellCastRequested(CastingActor, CastingDirection, SelectedSpell);

	if (SelectedSpell->LaunchesProjectile() && SelectedSpell->ProjectileActorClass)
	{
		const FVector ProjectileSpawnLocation = SpellReleasePoint->GetComponentLocation();

		const FRotator ProjectileSpawnRotation = CastingDirection.Rotation();

		FActorSpawnParameters ProjectileSpawnParameters;

		ProjectileSpawnParameters.Owner = CastingActor;

		ProjectileSpawnParameters.Instigator = Cast<APawn>(CastingActor);

		ProjectileSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASpellProjectile* CreatedSpellProjectile = GetWorld()->SpawnActor<ASpellProjectile>(SelectedSpell->ProjectileActorClass, ProjectileSpawnLocation, ProjectileSpawnRotation, ProjectileSpawnParameters);

		if (IsValid(CreatedSpellProjectile))
		{
			CreatedSpellProjectile->InitializeProjectile(CastingActor, SelectedSpell, CastingDirection);
		}
	}

	if (SelectedSpell->CastCooldownSeconds <= 0.0f)
	{
		FinishSpellRecovery();
	}
	else
	{
		GetWorldTimerManager().SetTimer(SpellRecoveryTimer, this, &AStaff::FinishSpellRecovery, SelectedSpell->CastCooldownSeconds, false);
	}

	return true;
}

ASpells* AStaff::GetSelectedSpell() const
{
	if (!SpellSlots.IsValidIndex(SelectedSpellIndex))
	{
		return nullptr;
	}

	if (!SpellSlots[SelectedSpellIndex])
	{
		return nullptr;
	}

	return SpellSlots[SelectedSpellIndex].GetDefaultObject();
}

int32 AStaff::GetSelectedSpellSlot() const
{
	return SelectedSpellIndex + 1;
}

void AStaff::SelectNextSpell()
{
	const int32 NumberOfSpellSlots = FMath::Min(SpellSlots.Num(), 4);

	if (NumberOfSpellSlots <= 0)
	{
		return;
	}

	for (int32 SlotOffset = 1; SlotOffset <= NumberOfSpellSlots; SlotOffset++)
	{
		const int32 CandidateSpellIndex = (SelectedSpellIndex + SlotOffset) % NumberOfSpellSlots;

		if (SpellSlots[CandidateSpellIndex])
		{
			SelectedSpellIndex = CandidateSpellIndex;

			return;
		}
	}
}

void AStaff::SelectPreviousSpell()
{
	const int32 NumberOfSpellSlots = FMath::Min(SpellSlots.Num(), 4);

	if (NumberOfSpellSlots <= 0)
	{
		return;
	}

	for (int32 SlotOffset = 1; SlotOffset <= NumberOfSpellSlots; SlotOffset++)
	{
		const int32 CandidateSpellIndex = (SelectedSpellIndex - SlotOffset + NumberOfSpellSlots) % NumberOfSpellSlots;

		if (SpellSlots[CandidateSpellIndex])
		{
			SelectedSpellIndex = CandidateSpellIndex;

			return;
		}
	}
}

bool AStaff::IsSpellReady() const
{
	return bSpellReady;
}

void AStaff::FinishSpellRecovery()
{
	bSpellReady = true;
}