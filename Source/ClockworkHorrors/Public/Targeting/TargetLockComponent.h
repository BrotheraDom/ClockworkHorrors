// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetLockComponent.generated.h"

class ACharacter;
class APlayerController;
class UTargetableComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnTargetLockChanged,
    AActor*, Target,
    bool, bLocked
);

/**
 * Reusable player-side Z-lock / target-lock component.
 *
 * Version 1 intentionally does not modify ABaseCharacter or AEnemy. Add this
 * component only to the duplicated test character Blueprint.
 */
UCLASS(ClassGroup = (Targeting), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UTargetLockComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UTargetLockComponent();

    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    /** Lock the best target, or unlock if already locked. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void ToggleTargetLock();

    /** Attempt to lock a specific actor. Returns true on success. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    bool LockTarget(AActor* NewTarget);

    /** Clear the current lock and restore the character's normal rotation settings. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void ClearTargetLock();

    /** Switch to the nearest valid target on the left side of the current target. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void SwitchTargetLeft();

    /** Switch to the nearest valid target on the right side of the current target. */
    UFUNCTION(BlueprintCallable, Category = "Targeting")
    void SwitchTargetRight();

    UFUNCTION(BlueprintPure, Category = "Targeting")
    bool IsTargetLocked() const;

    UFUNCTION(BlueprintPure, Category = "Targeting")
    AActor* GetCurrentTarget() const { return CurrentTarget; }

    /** Feed right-stick look input into the bounded lock-on combat camera. */
    void AddLockedCameraLookInput(const FVector2D& LookInput);

    /** Feed mouse delta into the same bounded combat camera without frame-time scaling. */
    void AddLockedCameraMouseLookInput(const FVector2D& LookInput);

    /** Fired when a target is acquired or released. Useful later for a target marker widget. */
    UPROPERTY(BlueprintAssignable, Category = "Targeting")
    FOnTargetLockChanged OnTargetLockChanged;

protected:
    /** Radius used when searching for Pawns that contain UTargetableComponent. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Selection", meta = (ClampMin = "100.0"))
    float SearchRadius = 2500.0f;

    /** The lock breaks if the target gets farther away than this. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Selection", meta = (ClampMin = "100.0"))
    float MaxLockDistance = 2000.0f;

    /** Maximum camera angle from center during initial target acquisition. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Selection", meta = (ClampMin = "1.0", ClampMax = "180.0"))
    float MaxTargetAngleDegrees = 70.0f;

    /** How strongly screen-center placement influences initial target choice. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Selection", meta = (ClampMin = "0.0"))
    float ScreenCenterWeight = 0.75f;

    /** How strongly distance influences initial target choice. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Selection", meta = (ClampMin = "0.0"))
    float DistanceWeight = 0.25f;

    /** Require an unobstructed Visibility trace to acquire/keep a target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Visibility")
    bool bRequireLineOfSight = true;

    /** Brief obstruction grace period so a pillar does not instantly break lock. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Visibility", meta = (ClampMin = "0.0"))
    float LostSightGracePeriod = 0.75f;

    /** Speed at which the controller/camera rotates toward the target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera", meta = (ClampMin = "0.0"))
    float CameraRotationInterpSpeed = 8.0f;

    /**
     * When enabled, pitch aims at a point between the player and target instead
     * of directly at the target. Yaw still points player -> target so locked
     * movement and facing keep their Zelda-style behavior.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Framing")
    bool bUseFramedCamera = true;

    /** Height above the character actor location used for the player side of the camera focus point. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Framing")
    float PlayerCameraFocusHeight = 60.0f;

    /**
     * Where the camera's pitch focus sits between player and target.
     * 0 = player focus point, 1 = target point. A value around 0.65-0.75
     * usually keeps both player and enemy visible.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Framing", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float TargetCameraFocusWeight = 0.70f;


    /** Dynamically pull the gameplay camera back as player/target separation increases. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance")
    bool bUseDynamicCameraDistance = true;

    /** Name of the gameplay spring arm. Your test character currently uses CameraBoom. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance")
    FName CameraBoomComponentName = TEXT("CameraBoom");

    /** Camera arm length used while fighting at close range. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance", meta = (ClampMin = "0.0"))
    float MinLockedCameraArmLength = 400.0f;

    /** Maximum camera arm length reached when the target is far away. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance", meta = (ClampMin = "0.0"))
    float MaxLockedCameraArmLength = 600.0f;

    /** Separation at which the camera begins pulling back. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance", meta = (ClampMin = "0.0"))
    float CameraZoomStartDistance = 400.0f;

    /** Separation at which the camera reaches Max Locked Camera Arm Length. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance", meta = (ClampMin = "1.0"))
    float CameraZoomFullDistance = 1600.0f;

    /** How quickly spring-arm length interpolates toward its desired lock-on distance. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Distance", meta = (ClampMin = "0.0"))
    float CameraArmLengthInterpSpeed = 5.0f;

    /** Raise the gameplay camera while locked so the player does not block the target. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Height")
    bool bUseDynamicCameraHeight = true;

    /** Extra camera height used at close range, added on top of the boom's original SocketOffset.Z. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Height")
    float MinLockedCameraHeightOffset = 35.0f;

    /** Extra camera height used at long range. 75 matched the test angle you liked. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Height")
    float MaxLockedCameraHeightOffset = 75.0f;

    /** How quickly the boom height interpolates toward the desired lock-on height. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Height", meta = (ClampMin = "0.0"))
    float CameraHeightInterpSpeed = 5.0f;

    /** Added to the calculated camera pitch while locked. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera")
    float CameraPitchOffset = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera")
    float MinCameraPitch = -55.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera")
    float MaxCameraPitch = 45.0f;

    /** Allow the right stick to orbit the combat camera without breaking target-facing movement. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look")
    bool bAllowManualCameraWhileLocked = true;

    /** Maximum horizontal combat-camera orbit from the locked target direction. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0", ClampMax = "90.0"))
    float MaxLockedCameraYawOffset = 22.0f;

    /** Maximum vertical manual pitch offset from the automatic lock framing. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0", ClampMax = "60.0"))
    float MaxLockedCameraPitchOffset = 12.0f;

    /** Horizontal right-stick camera speed in degrees per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedCameraYawSpeed = 70.0f;

    /** Vertical right-stick camera speed in degrees per second. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedCameraPitchSpeed = 55.0f;

    /** Allow IA_MouseLook to nudge the bounded lock-on camera too. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look")
    bool bAllowMouseCameraWhileLocked = true;

    /** Degrees of bounded yaw offset added per mouse X input unit. Mouse delta is already frame-based, so no DeltaTime is applied. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedMouseYawSensitivity = 0.35f;

    /** Degrees of bounded pitch offset added per mouse Y input unit. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedMousePitchSensitivity = 0.30f;

    /** Ignore tiny analog values before accumulating a combat-camera offset. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float LockedCameraInputDeadZone = 0.08f;

    /** Return the combat camera toward center after the player releases the stick. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look")
    bool bAutoRecenterLockedCamera = true;

    /** Delay after the last right-stick input before automatic recentering begins. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedCameraRecenterDelay = 0.20f;

    /** How quickly manual camera offsets interpolate back toward zero. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera|Manual Look", meta = (ClampMin = "0.0"))
    float LockedCameraRecenterSpeed = 3.5f;

    /** Prevent the normal controller look binding from fighting the lock camera. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting|Camera")
    bool bBlockManualLookWhileLocked = true;

private:
    void RefreshOwnerReferences();

    AActor* FindBestTarget() const;

    void GatherCandidateTargets(TArray<AActor*>& OutTargets) const;

    UTargetableComponent* GetTargetableComponent(AActor* Target) const;

    bool IsTargetEligible(AActor* Target, bool bCheckViewAngle) const;

    bool HasLineOfSightToTarget(AActor* Target) const;

    bool ProjectTargetToScreen(AActor* Target, FVector2D& OutScreenPosition) const;

    FVector GetTargetLocation(AActor* Target) const;

    void UpdateLockedTarget(float DeltaTime);

    void UpdateDynamicCameraDistance(float DeltaTime, float TargetDistance);

    void UpdateDynamicCameraHeight(float DeltaTime, float TargetDistance);

    void UpdateLockedCameraOffsets(float DeltaTime);

    void ApplyLockedCameraSettings();

    void ApplyLockedMovementSettings();

    void SaveCameraSettings();

    void RestoreCameraSettings();

    void RestoreMovementSettings();

    void SwitchTarget(float Direction);

private:
    UPROPERTY(Transient)
    TObjectPtr<AActor> CurrentTarget = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<ACharacter> OwnerCharacter = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<APlayerController> OwnerPlayerController = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<USpringArmComponent> GameplayCameraBoom = nullptr;

    float LostSightTime = 0.0f;

    bool bSavedCameraSettings = false;
    float SavedCameraArmLength = 0.0f;
    FVector SavedCameraSocketOffset = FVector::ZeroVector;
    FRotator SavedCameraBoomRelativeRotation = FRotator::ZeroRotator;
    bool bSavedCameraBoomUsePawnControlRotation = true;

    float LockedCameraYawOffset = 0.0f;
    float LockedCameraPitchOffset = 0.0f;
    float LastLockedCameraInputTime = -1000.0f;

    bool bSavedRotationSettings = false;
    bool bSavedUseControllerRotationYaw = false;
    bool bSavedOrientRotationToMovement = true;
    bool bSavedUseControllerDesiredRotation = false;
    bool bBlockedLookInput = false;
};
