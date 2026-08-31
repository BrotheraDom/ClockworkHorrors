// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Staff.h"

#include "SpellProjectile.h"
#include "WeaponPickup.h"
#include "Utils/HealthComponent.h"

#include "Components/CharacterAnimationComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
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
	AnimationHolder = nullptr;

	SpellCooldownEndTimes.SetNumZeroed(4);
	bStaffInputBindingsCreated = false;
	bIsCharging = false;
	ChargeStartTimeSeconds = 0.0f;

	SelectedSpellIndex = 0;
}

void AStaff::BeginPlay()
{
	Super::BeginPlay();

	if (SpellSlots.Num() != 4)
	{
		SpellSlots.SetNum(4);
	}

	if (SpellCooldownEndTimes.Num() != 4)
	{
		SpellCooldownEndTimes.SetNumZeroed(4);
	}

	WeaponPickup = FindComponentByClass<UWeaponPickup>();

	if (IsValid(WeaponPickup))
	{
		WeaponPickup->OnWeaponEquippedStateChanged.AddDynamic(this, &AStaff::HandleStaffWeaponEquippedStateChanged);
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

void AStaff::Attack()
{
	ABaseWeapon::Attack();

	if (!ParentPawn)
	{
		return;
	}

	if (!CanAttack())
	{
		return;
	}

	if (!IsSpellReady())
	{
		return;
	}

	ASpells* SelectedSpell = GetSelectedSpell();

	if (!IsValid(SelectedSpell))
	{
		return;
	}

	if (SelectedSpell->bChargeableSpell)
	{
		if (bIsCharging)
		{
			return;
		}

		bIsCharging = true;
		ChargeStartTimeSeconds = GetWorld()->GetTimeSeconds();

		return;
	}

	FVector CastingDirection = ParentPawn->GetActorForwardVector();

	APlayerController* PlayerController = Cast<APlayerController>(ParentPawn->GetController());

	if (IsValid(PlayerController))
	{
		CastingDirection = PlayerController->GetControlRotation().Vector();
	}

	AttemptSpellCast(ParentPawn, CastingDirection);
}

void AStaff::ReleaseSpellCharge()
{
	if (!bIsCharging)
	{
		return;
	}

	if (!ParentPawn)
	{
		bIsCharging = false;
		return;
	}

	if (!CanAttack())
	{
		bIsCharging = false;
		return;
	}

	if (!IsStaffControlledByPlayer())
	{
		bIsCharging = false;
		return;
	}

	ASpells* SelectedSpell = GetSelectedSpell();

	if (!IsValid(SelectedSpell) || !SelectedSpell->bChargeableSpell)
	{
		bIsCharging = false;
		return;
	}

	const float ChargePercent = GetCurrentChargePercent();

	bIsCharging = false;

	FVector CastingDirection = ParentPawn->GetActorForwardVector();

	APlayerController* PlayerController = Cast<APlayerController>(ParentPawn->GetController());

	if (IsValid(PlayerController))
	{
		CastingDirection = PlayerController->GetControlRotation().Vector();
	}

	AttemptSpellCastCharged(ParentPawn, CastingDirection, ChargePercent);
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

void AStaff::HandleStaffWeaponEquippedStateChanged(bool bWeaponEquipped)
{
	UpdateStaffInputConsumption();

	if (!IsValid(WeaponPickup))
	{
		return;
	}

	if (bWeaponEquipped)
	{
		AnimationHolder = WeaponPickup->GetEquippedHolder();
	}
	else
	{
		bIsCharging = false;
	}

	if (!IsValid(AnimationHolder))
	{
		return;
	}

	if (
		UCharacterAnimationComponent* AnimationComponent =
		AnimationHolder->FindComponentByClass<UCharacterAnimationComponent>()
		)
	{
		//if (bWeaponEquipped)
		//{
		//	AnimationComponent->SetAnimationStance(AnimationStance);
		//}
		//else
		//{
		//	AnimationComponent->ResetAnimationStance();
		//}
	}

	if (!bWeaponEquipped)
	{
		AnimationHolder = nullptr;
	}
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
		if ((InputBinding.Chord.Key == EKeys::MouseScrollUp || InputBinding.Chord.Key == EKeys::MouseScrollDown) && InputBinding.KeyEvent == IE_Pressed)
		{
			InputBinding.bConsumeInput = bShouldConsumeStaffInput;
		}
	}
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
	return AttemptSpellCastCharged(CastingActor, CastingDirection, 0.0f);
}

bool AStaff::AttemptSpellCastCharged(AActor* CastingActor, FVector CastingDirection, float ChargePercent)
{
	if (!IsValid(CastingActor))
	{
		return false;
	}

	if (!IsSpellReady())
	{
		return false;
	}

	ASpells* SelectedSpell = GetSelectedSpell();

	const int32 CastSpellIndex = SelectedSpellIndex;

	if (!IsValid(SelectedSpell))
	{
		return false;
	}

	if (CastingDirection.IsNearlyZero())
	{
		CastingDirection = CastingActor->GetActorForwardVector();
	}

	CastingDirection.Normalize();

	bIsCharging = false;

	ChargePercent = FMath::Clamp(ChargePercent, 0.0f, 1.0f);

	SpellCastRequested(CastingActor, CastingDirection, SelectedSpell);

	if (SelectedSpell->CastsOnCaster() && SelectedSpell->bRestoresImpactHealth)
	{
		UHealthComponent* HealthComponent = CastingActor->FindComponentByClass<UHealthComponent>();

		if (IsValid(HealthComponent) && HealthComponent->CanHeal())
		{
			HealthComponent->Heal(SelectedSpell->ImpactHealthRestoration);
		}
	}

	if (SelectedSpell->LaunchesProjectile() && SelectedSpell->ProjectileActorClass)
	{
		const FVector ProjectileSpawnLocation = SpellReleasePoint->GetComponentLocation();

		const int32 NumberOfProjectiles = SelectedSpell->bProjectileMultishot
			? FMath::Max(SelectedSpell->ProjectileCount, 2)
			: 1;

		const float TotalSpread = SelectedSpell->bProjectileMultishot
			? SelectedSpell->ProjectileSpreadAngle
			: 0.0f;

		const FRotator BaseProjectileSpawnRotation = CastingDirection.Rotation();

		TArray<ASpellProjectile*> CreatedSpellProjectiles;

		for (int32 ProjectileIndex = 0; ProjectileIndex < NumberOfProjectiles; ProjectileIndex++)
		{
			float YawOffset = 0.0f;

			if (NumberOfProjectiles > 1)
			{
				const float SpreadPercent =
					static_cast<float>(ProjectileIndex) /
					static_cast<float>(NumberOfProjectiles - 1);

				YawOffset = FMath::Lerp(
					-TotalSpread * 0.5f,
					TotalSpread * 0.5f,
					SpreadPercent
				);
			}

			FRotator ProjectileSpawnRotation = BaseProjectileSpawnRotation;
			ProjectileSpawnRotation.Yaw += YawOffset;

			const FVector ProjectileDirection = ProjectileSpawnRotation.Vector();

			FActorSpawnParameters ProjectileSpawnParameters;

			ProjectileSpawnParameters.Owner = CastingActor;

			ProjectileSpawnParameters.Instigator = Cast<APawn>(CastingActor);

			ProjectileSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			ASpellProjectile* CreatedSpellProjectile = GetWorld()->SpawnActor<ASpellProjectile>(
				SelectedSpell->ProjectileActorClass,
				ProjectileSpawnLocation,
				ProjectileSpawnRotation,
				ProjectileSpawnParameters
			);

			if (IsValid(CreatedSpellProjectile))
			{
				CreatedSpellProjectile->InitializeProjectile(
					CastingActor,
					SelectedSpell,
					ProjectileDirection,
					ChargePercent
				);

				CreatedSpellProjectiles.Add(CreatedSpellProjectile);
			}
		}

		for (int32 FirstProjectileIndex = 0; FirstProjectileIndex < CreatedSpellProjectiles.Num(); FirstProjectileIndex++)
		{
			for (int32 SecondProjectileIndex = FirstProjectileIndex + 1; SecondProjectileIndex < CreatedSpellProjectiles.Num(); SecondProjectileIndex++)
			{
				ASpellProjectile* FirstProjectile = CreatedSpellProjectiles[FirstProjectileIndex];
				ASpellProjectile* SecondProjectile = CreatedSpellProjectiles[SecondProjectileIndex];

				if (IsValid(FirstProjectile) && IsValid(FirstProjectile->CollisionSphere) && IsValid(SecondProjectile))
				{
					FirstProjectile->CollisionSphere->IgnoreActorWhenMoving(SecondProjectile, true);
				}

				if (IsValid(SecondProjectile) && IsValid(SecondProjectile->CollisionSphere) && IsValid(FirstProjectile))
				{
					SecondProjectile->CollisionSphere->IgnoreActorWhenMoving(FirstProjectile, true);
				}
			}
		}
	}

	if (SpellCooldownEndTimes.IsValidIndex(CastSpellIndex))
	{
		SpellCooldownEndTimes[CastSpellIndex] = GetWorld()->GetTimeSeconds() + FMath::Max(SelectedSpell->CastCooldownSeconds, 0.0f);
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
	if (bIsCharging)
	{
		return;
	}

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
	if (bIsCharging)
	{
		return;
	}

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
	if (!SpellCooldownEndTimes.IsValidIndex(SelectedSpellIndex))
	{
		return true;
	}

	if (!GetWorld())
	{
		return true;
	}

	return GetWorld()->GetTimeSeconds() >= SpellCooldownEndTimes[SelectedSpellIndex];
}

bool AStaff::IsChargingSpell() const
{
	return bIsCharging;
}

float AStaff::GetCurrentChargePercent() const
{
	if (!bIsCharging)
	{
		return 0.0f;
	}

	ASpells* SelectedSpell = GetSelectedSpell();

	if (!IsValid(SelectedSpell) || !SelectedSpell->bChargeableSpell)
	{
		return 0.0f;
	}

	const float MinimumChargeTime = FMath::Max(SelectedSpell->MinimumChargeTimeSeconds, 0.0f);
	const float MaximumChargeTime = FMath::Max(SelectedSpell->MaximumChargeTimeSeconds, MinimumChargeTime);
	const float HeldTime = FMath::Max(GetWorld()->GetTimeSeconds() - ChargeStartTimeSeconds, 0.0f);

	if (MaximumChargeTime - MinimumChargeTime <= KINDA_SMALL_NUMBER)
	{
		return HeldTime >= MinimumChargeTime ? 1.0f : 0.0f;
	}

	return FMath::Clamp(
		(HeldTime - MinimumChargeTime) /
		(MaximumChargeTime - MinimumChargeTime),
		0.0f,
		1.0f
	);
}
