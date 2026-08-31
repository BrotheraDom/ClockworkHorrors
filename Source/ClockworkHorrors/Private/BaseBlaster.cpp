// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "BaseBlaster.h"
#include "Components/BoxComponent.h"
#include "Projectile.h"
#include "WeaponPickup.h"
#include "Components/InputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Utils/InventoryItemDataAsset.h"

// Sets default values
ABaseBlaster::ABaseBlaster()
{
    AnimationStance = ECharacterAnimationStance::Rifle;

    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    ActionHappening = false;

}

// Called when the game starts or when spawned
void ABaseBlaster::BeginPlay()
{
    Super::BeginPlay();
    //upcast
    AActor* Actor = this->GetParentActor();
    // downcast
    ParentPawn = Cast<APawn>(Actor);
    if (!ParentPawn)
    {

    }
    if (UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>())
    {
        if (Pickup->ItemDataAsset->Ammo != 0)
        {
            currentAmmo = Pickup->ItemDataAsset->Ammo;
        }
        else
        {
            reloadAmmo();
        }
    }
}

void ABaseBlaster::Attack()
{
    ABaseWeapon::Attack();

    if (!CanAttack())
    {
        return;
    }

    if (!ParentPawn || !skeletalMesh || !projectileClass)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Blaster cannot fire: ParentPawn, skeletalMesh, or projectileClass is missing.")
        );
        return;
    }

    APlayerController* PlayerController =
        Cast<APlayerController>(ParentPawn->GetController());

    if (!PlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("Blaster cannot fire: PlayerController is null."));
        return;
    }

    FVector CameraLocation;
    FRotator CameraRotation;

    PlayerController->GetPlayerViewPoint(
        CameraLocation,
        CameraRotation
    );

    constexpr float TraceDistance = 100000.0f;

    const FVector CameraTraceEnd =
        CameraLocation +
        (CameraRotation.Vector() * TraceDistance);

    FHitResult CameraHit;

    FCollisionQueryParams TraceParams;
    TraceParams.AddIgnoredActor(ParentPawn);
    TraceParams.AddIgnoredActor(this);

    const bool bCameraHit = GetWorld()->LineTraceSingleByChannel(
        CameraHit,
        CameraLocation,
        CameraTraceEnd,
        ECC_Visibility,
        TraceParams
    );

    const FVector AimPoint =
        bCameraHit ? CameraHit.ImpactPoint : CameraTraceEnd;

    const FVector MuzzleLocation =
        skeletalMesh->GetSocketLocation(FName("MuzzleSocket"));

    const FRotator ShootRotation =
        (AimPoint - MuzzleLocation).Rotation();

    FActorSpawnParameters Params;
    Params.Instigator = ParentPawn;
    Params.Owner = ParentPawn;

    AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(
        projectileClass,
        MuzzleLocation,
        ShootRotation,
        Params
    );

    projectile = Cast<AProjectile>(SpawnedActor);

    if (SpawnedActor)
    {
        FTimerHandle ShootTimer;

        ActionHappening = true;

        GetWorld()->GetTimerManager().SetTimer(
            ShootTimer,
            this,
            &ABaseBlaster::ActionStopped,
            1.0f
        );

        OnDelegateInstance.Broadcast();
        useAmmo();
    }
}


const bool ABaseBlaster::CanAttack()
{
    return !ActionHappening && Alive && currentAmmo > 0;
}

void ABaseBlaster::useAmmo()
{
    currentAmmo = FMath::Max(currentAmmo - 1, 0);
    OnAmmoChange.Broadcast(currentAmmo, maxAmmo);
    UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>();
    Pickup->ItemDataAsset->Ammo = currentAmmo;
}



void ABaseBlaster::requestReload()
{
    if (!ActionHappening)
    {
        ActionHappening = true;
    }
    OnReloadStart.Broadcast();
}

void ABaseBlaster::reloadAmmo()
{
    currentAmmo = maxAmmo;
    UWeaponPickup* Pickup = FindComponentByClass<UWeaponPickup>();
    Pickup->ItemDataAsset->Ammo = currentAmmo;
    OnAmmoChange.Broadcast(currentAmmo, maxAmmo);
}


int ABaseBlaster::getCurrAmmo()
{
    return currentAmmo;
}

int ABaseBlaster::getMaxAmmo()
{
    return maxAmmo;
}

void ABaseBlaster::setMaxAmmo(int newMax)
{
    maxAmmo = newMax;
}

void ABaseBlaster::setCurrAmmo(int curAmmo)
{
    currentAmmo = curAmmo;
}
