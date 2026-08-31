// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Targeting/TargetLockComponent.h"

#include "Targeting/TargetableComponent.h"

#include "Engine/World.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UTargetLockComponent::UTargetLockComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTargetLockComponent::BeginPlay()
{
    Super::BeginPlay();
    RefreshOwnerReferences();
}

void UTargetLockComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    ClearTargetLock();
    Super::EndPlay(EndPlayReason);
}

void UTargetLockComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!CurrentTarget)
    {
        return;
    }

    // If the target was destroyed externally, IsValid becomes false before
    // the raw reference necessarily becomes null. Restore movement/look state.
    if (!IsValid(CurrentTarget))
    {
        ClearTargetLock();
        return;
    }

    RefreshOwnerReferences();
    UpdateLockedTarget(DeltaTime);
}

void UTargetLockComponent::RefreshOwnerReferences()
{
    if (!OwnerCharacter)
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
    }

    if (OwnerCharacter)
    {
        OwnerPlayerController = Cast<APlayerController>(OwnerCharacter->GetController());

        if (!GameplayCameraBoom)
        {
            TInlineComponentArray<USpringArmComponent*> SpringArms;
            OwnerCharacter->GetComponents(SpringArms);

            for (USpringArmComponent* SpringArm : SpringArms)
            {
                if (SpringArm && SpringArm->GetFName() == CameraBoomComponentName)
                {
                    GameplayCameraBoom = SpringArm;
                    break;
                }
            }
        }
    }
}

void UTargetLockComponent::ToggleTargetLock()
{
    RefreshOwnerReferences();

    if (IsTargetLocked())
    {
        ClearTargetLock();
        return;
    }

    if (AActor* BestTarget = FindBestTarget())
    {
        LockTarget(BestTarget);
    }
}

bool UTargetLockComponent::LockTarget(AActor* NewTarget)
{
    RefreshOwnerReferences();

    if (!OwnerCharacter || !OwnerPlayerController)
    {
        UE_LOG(LogTemp, Warning, TEXT("TargetLock: owner is not a locally controlled Character/PlayerController."));
        return false;
    }

    if (!IsTargetEligible(NewTarget, false))
    {
        return false;
    }

    if (CurrentTarget == NewTarget)
    {
        return true;
    }

    const bool bWasAlreadyLocked = IsValid(CurrentTarget);

    if (bWasAlreadyLocked)
    {
        OnTargetLockChanged.Broadcast(CurrentTarget, false);
    }

    CurrentTarget = NewTarget;
    LostSightTime = 0.0f;

    if (!bWasAlreadyLocked)
    {
        LockedCameraYawOffset = 0.0f;
        LockedCameraPitchOffset = 0.0f;
        LastLockedCameraInputTime = -1000.0f;
    }

    ApplyLockedMovementSettings();
    SaveCameraSettings();
    ApplyLockedCameraSettings();

    // The normal controller look callback is still bound. Ignore its yaw/pitch
    // contribution while locked; the test controller separately forwards the
    // right-stick value to AddLockedCameraLookInput().
    if ((bBlockManualLookWhileLocked || bAllowManualCameraWhileLocked) && !bBlockedLookInput)
    {
        OwnerPlayerController->SetIgnoreLookInput(true);
        bBlockedLookInput = true;
    }

    OnTargetLockChanged.Broadcast(CurrentTarget, true);

    UE_LOG(LogTemp, Log, TEXT("TargetLock: locked %s"), *GetNameSafe(CurrentTarget));
    return true;
}

void UTargetLockComponent::ClearTargetLock()
{
    AActor* OldTarget = CurrentTarget;
    CurrentTarget = nullptr;
    LostSightTime = 0.0f;

    RestoreMovementSettings();
    RestoreCameraSettings();

    LockedCameraYawOffset = 0.0f;
    LockedCameraPitchOffset = 0.0f;
    LastLockedCameraInputTime = -1000.0f;

    if (bBlockedLookInput && OwnerPlayerController)
    {
        OwnerPlayerController->SetIgnoreLookInput(false);
        bBlockedLookInput = false;
    }

    if (IsValid(OldTarget))
    {
        OnTargetLockChanged.Broadcast(OldTarget, false);
        UE_LOG(LogTemp, Log, TEXT("TargetLock: released %s"), *GetNameSafe(OldTarget));
    }
}

void UTargetLockComponent::SwitchTargetLeft()
{
    SwitchTarget(-1.0f);
}

void UTargetLockComponent::SwitchTargetRight()
{
    SwitchTarget(1.0f);
}

bool UTargetLockComponent::IsTargetLocked() const
{
    return IsValid(CurrentTarget);
}

void UTargetLockComponent::AddLockedCameraLookInput(const FVector2D& LookInput)
{
    if (!bAllowManualCameraWhileLocked || !IsTargetLocked() || !GetWorld())
    {
        return;
    }

    if (LookInput.SizeSquared() < FMath::Square(LockedCameraInputDeadZone))
    {
        return;
    }

    const float DeltaTime = GetWorld()->GetDeltaSeconds();

    LockedCameraYawOffset = FMath::Clamp(
        LockedCameraYawOffset + (LookInput.X * LockedCameraYawSpeed * DeltaTime),
        -MaxLockedCameraYawOffset,
        MaxLockedCameraYawOffset
    );

    LockedCameraPitchOffset = FMath::Clamp(
        LockedCameraPitchOffset + (LookInput.Y * LockedCameraPitchSpeed * DeltaTime),
        -MaxLockedCameraPitchOffset,
        MaxLockedCameraPitchOffset
    );

    LastLockedCameraInputTime = GetWorld()->GetTimeSeconds();
}

void UTargetLockComponent::AddLockedCameraMouseLookInput(const FVector2D& LookInput)
{
    if (!bAllowManualCameraWhileLocked || !bAllowMouseCameraWhileLocked || !IsTargetLocked() || !GetWorld())
    {
        return;
    }

    if (LookInput.IsNearlyZero())
    {
        return;
    }

    // Mouse actions provide per-frame delta already. Unlike an analog stick,
    // multiplying this value by DeltaTime would make mouse response weak and
    // frame-rate dependent, so use a direct sensitivity multiplier.
    LockedCameraYawOffset = FMath::Clamp(
        LockedCameraYawOffset + (LookInput.X * LockedMouseYawSensitivity),
        -MaxLockedCameraYawOffset,
        MaxLockedCameraYawOffset
    );

    LockedCameraPitchOffset = FMath::Clamp(
        LockedCameraPitchOffset - (LookInput.Y * LockedMousePitchSensitivity),
        -MaxLockedCameraPitchOffset,
        MaxLockedCameraPitchOffset
    );

    LastLockedCameraInputTime = GetWorld()->GetTimeSeconds();
}

AActor* UTargetLockComponent::FindBestTarget() const
{
    if (!OwnerCharacter || !OwnerPlayerController)
    {
        return nullptr;
    }

    TArray<AActor*> Candidates;
    GatherCandidateTargets(Candidates);

    int32 ViewportX = 0;
    int32 ViewportY = 0;
    OwnerPlayerController->GetViewportSize(ViewportX, ViewportY);

    const FVector2D ScreenCenter(
        static_cast<float>(ViewportX) * 0.5f,
        static_cast<float>(ViewportY) * 0.5f
    );

    const float MaxScreenDistance = FMath::Max(
        FVector2D(ScreenCenter.X, ScreenCenter.Y).Size(),
        1.0f
    );

    float BestScore = -TNumericLimits<float>::Max();
    AActor* BestTarget = nullptr;

    for (AActor* Candidate : Candidates)
    {
        if (!IsTargetEligible(Candidate, true))
        {
            continue;
        }

        FVector2D ScreenPosition;
        if (!ProjectTargetToScreen(Candidate, ScreenPosition))
        {
            continue;
        }

        const float ScreenDistance = FVector2D::Distance(ScreenPosition, ScreenCenter);
        const float ScreenScore = 1.0f - FMath::Clamp(ScreenDistance / MaxScreenDistance, 0.0f, 1.0f);

        const float Distance = FVector::Dist(
            OwnerCharacter->GetActorLocation(),
            GetTargetLocation(Candidate)
        );

        const float DistanceScore = 1.0f - FMath::Clamp(Distance / MaxLockDistance, 0.0f, 1.0f);

        float Priority = 0.0f;
        if (const UTargetableComponent* Targetable = GetTargetableComponent(Candidate))
        {
            Priority = Targetable->GetTargetPriority();
        }

        const float Score =
            (ScreenScore * ScreenCenterWeight)
            + (DistanceScore * DistanceWeight)
            + Priority;

        if (Score > BestScore)
        {
            BestScore = Score;
            BestTarget = Candidate;
        }
    }

    return BestTarget;
}

void UTargetLockComponent::GatherCandidateTargets(TArray<AActor*>& OutTargets) const
{
    OutTargets.Reset();

    if (!OwnerCharacter || !GetWorld())
    {
        return;
    }

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TargetLockSearch), false);
    QueryParams.AddIgnoredActor(OwnerCharacter);

    TArray<FOverlapResult> OverlapResults;

    const bool bFoundAny = GetWorld()->OverlapMultiByObjectType(
        OverlapResults,
        OwnerCharacter->GetActorLocation(),
        FQuat::Identity,
        ObjectQueryParams,
        FCollisionShape::MakeSphere(SearchRadius),
        QueryParams
    );

    if (!bFoundAny)
    {
        return;
    }

    TSet<AActor*> AddedActors;

    for (const FOverlapResult& Result : OverlapResults)
    {
        AActor* Candidate = Result.GetActor();

        if (!Candidate || AddedActors.Contains(Candidate))
        {
            continue;
        }

        if (GetTargetableComponent(Candidate))
        {
            AddedActors.Add(Candidate);
            OutTargets.Add(Candidate);
        }
    }
}

UTargetableComponent* UTargetLockComponent::GetTargetableComponent(AActor* Target) const
{
    return Target ? Target->FindComponentByClass<UTargetableComponent>() : nullptr;
}

bool UTargetLockComponent::IsTargetEligible(AActor* Target, bool bCheckViewAngle) const
{
    if (!Target || !OwnerCharacter || Target == OwnerCharacter)
    {
        return false;
    }

    const UTargetableComponent* Targetable = GetTargetableComponent(Target);
    if (!Targetable || !Targetable->CanBeTargeted())
    {
        return false;
    }

    const FVector TargetLocation = Targetable->GetTargetLocation();
    const float Distance = FVector::Dist(OwnerCharacter->GetActorLocation(), TargetLocation);

    if (Distance > MaxLockDistance)
    {
        return false;
    }

    if (bRequireLineOfSight && !HasLineOfSightToTarget(Target))
    {
        return false;
    }

    if (bCheckViewAngle)
    {
        if (!OwnerPlayerController)
        {
            return false;
        }

        FVector ViewLocation;
        FRotator ViewRotation;
        OwnerPlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

        const FVector ToTarget = (TargetLocation - ViewLocation).GetSafeNormal();
        const float Dot = FVector::DotProduct(ViewRotation.Vector(), ToTarget);
        const float AngleDegrees = FMath::RadiansToDegrees(
            FMath::Acos(FMath::Clamp(Dot, -1.0f, 1.0f))
        );

        if (AngleDegrees > MaxTargetAngleDegrees)
        {
            return false;
        }
    }

    return true;
}

bool UTargetLockComponent::HasLineOfSightToTarget(AActor* Target) const
{
    if (!Target || !OwnerCharacter || !OwnerPlayerController || !GetWorld())
    {
        return false;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    OwnerPlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(TargetLockVisibility), true);
    QueryParams.AddIgnoredActor(OwnerCharacter);

    FHitResult HitResult;
    const bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        ViewLocation,
        GetTargetLocation(Target),
        ECC_Visibility,
        QueryParams
    );

    // No blocking hit means the target point is visible. If we do hit,
    // the hit is also valid when the first blocking actor is the target itself.
    return !bHit || HitResult.GetActor() == Target;
}

bool UTargetLockComponent::ProjectTargetToScreen(
    AActor* Target,
    FVector2D& OutScreenPosition
) const
{
    if (!OwnerPlayerController || !Target)
    {
        return false;
    }

    return OwnerPlayerController->ProjectWorldLocationToScreen(
        GetTargetLocation(Target),
        OutScreenPosition,
        true
    );
}

FVector UTargetLockComponent::GetTargetLocation(AActor* Target) const
{
    if (const UTargetableComponent* Targetable = GetTargetableComponent(Target))
    {
        return Targetable->GetTargetLocation();
    }

    return Target ? Target->GetActorLocation() : FVector::ZeroVector;
}

void UTargetLockComponent::UpdateLockedTarget(float DeltaTime)
{
    if (!CurrentTarget || !OwnerCharacter || !OwnerPlayerController)
    {
        ClearTargetLock();
        return;
    }

    const UTargetableComponent* Targetable = GetTargetableComponent(CurrentTarget);
    if (!Targetable || !Targetable->CanBeTargeted())
    {
        ClearTargetLock();
        return;
    }

    const FVector OwnerLocation = OwnerCharacter->GetActorLocation();
    const FVector TargetLocation = Targetable->GetTargetLocation();

    const float TargetDistance = FVector::Dist(OwnerLocation, TargetLocation);

    if (TargetDistance > MaxLockDistance)
    {
        ClearTargetLock();
        return;
    }

    UpdateDynamicCameraDistance(DeltaTime, TargetDistance);
    UpdateDynamicCameraHeight(DeltaTime, TargetDistance);
    UpdateLockedCameraOffsets(DeltaTime);

    if (bRequireLineOfSight)
    {
        if (HasLineOfSightToTarget(CurrentTarget))
        {
            LostSightTime = 0.0f;
        }
        else
        {
            LostSightTime += DeltaTime;

            if (LostSightTime >= LostSightGracePeriod)
            {
                ClearTargetLock();
                return;
            }
        }
    }

    // Horizontal yaw points exactly from player -> target. This is important
    // because AClockworkPlayerController::Move() uses ControlRotation to build
    // its forward/right movement axes. While locked, W/S therefore move
    // toward/away from the target and A/D strafe around it.
    FVector FlatToTarget = TargetLocation - OwnerLocation;
    FlatToTarget.Z = 0.0f;

    if (FlatToTarget.IsNearlyZero())
    {
        return;
    }

    const float DesiredYaw = FlatToTarget.Rotation().Yaw;

    FVector ViewLocation;
    FRotator ViewRotation;
    OwnerPlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);

    // Keep yaw tied directly to player -> target because the existing movement
    // code builds its W/S/A/D axes from ControlRotation. For camera framing we
    // only change the pitch focus point. This preserves the working locked
    // movement while giving the camera room to show both characters.
    FVector CameraFocusLocation = TargetLocation;

    if (bUseFramedCamera)
    {
        const FVector PlayerFocusLocation =
            OwnerLocation + (FVector::UpVector * PlayerCameraFocusHeight);

        CameraFocusLocation = FMath::Lerp(
            PlayerFocusLocation,
            TargetLocation,
            TargetCameraFocusWeight
        );
    }

    float DesiredPitch =
        (CameraFocusLocation - ViewLocation).Rotation().Pitch + CameraPitchOffset;

    DesiredPitch = FMath::Clamp(DesiredPitch, MinCameraPitch, MaxCameraPitch);

    // Control yaw remains tied to the target so the existing movement code
    // still interprets W/S as toward/away and A/D as strafe. Manual camera
    // freedom is applied to the spring arm instead of ControlRotation.
    const FRotator DesiredControlRotation(DesiredPitch, DesiredYaw, 0.0f);
    const FRotator CurrentControlRotation = OwnerPlayerController->GetControlRotation();

    const FRotator NewControlRotation = FMath::RInterpTo(
        CurrentControlRotation,
        DesiredControlRotation,
        DeltaTime,
        CameraRotationInterpSpeed
    );

    OwnerPlayerController->SetControlRotation(NewControlRotation);

    if (bAllowManualCameraWhileLocked && GameplayCameraBoom)
    {
        const float CharacterYaw = OwnerCharacter->GetActorRotation().Yaw;
        const float TargetRelativeYaw =
            FMath::FindDeltaAngleDegrees(CharacterYaw, DesiredYaw);

        const float BoomPitch = FMath::Clamp(
            DesiredPitch + LockedCameraPitchOffset,
            MinCameraPitch,
            MaxCameraPitch
        );

        const FRotator DesiredBoomRotation(
            BoomPitch,
            TargetRelativeYaw + LockedCameraYawOffset,
            0.0f
        );

        const FRotator NewBoomRotation = FMath::RInterpTo(
            GameplayCameraBoom->GetRelativeRotation(),
            DesiredBoomRotation,
            DeltaTime,
            CameraRotationInterpSpeed
        );

        GameplayCameraBoom->SetRelativeRotation(NewBoomRotation);
    }
}

void UTargetLockComponent::UpdateDynamicCameraDistance(float DeltaTime, float TargetDistance)
{
    if (!bUseDynamicCameraDistance || !GameplayCameraBoom)
    {
        return;
    }

    const float ZoomRange = FMath::Max(CameraZoomFullDistance - CameraZoomStartDistance, 1.0f);
    const float ZoomAlpha = FMath::Clamp(
        (TargetDistance - CameraZoomStartDistance) / ZoomRange,
        0.0f,
        1.0f
    );

    const float DesiredArmLength = FMath::Lerp(
        MinLockedCameraArmLength,
        MaxLockedCameraArmLength,
        ZoomAlpha
    );

    GameplayCameraBoom->TargetArmLength = FMath::FInterpTo(
        GameplayCameraBoom->TargetArmLength,
        DesiredArmLength,
        DeltaTime,
        CameraArmLengthInterpSpeed
    );
}

void UTargetLockComponent::UpdateDynamicCameraHeight(float DeltaTime, float TargetDistance)
{
    if (!bUseDynamicCameraHeight || !GameplayCameraBoom || !bSavedCameraSettings)
    {
        return;
    }

    // Reuse the same distance range as dynamic zoom so height and pull-back
    // feel like one coordinated combat-camera movement.
    const float HeightRange = FMath::Max(CameraZoomFullDistance - CameraZoomStartDistance, 1.0f);
    const float HeightAlpha = FMath::Clamp(
        (TargetDistance - CameraZoomStartDistance) / HeightRange,
        0.0f,
        1.0f
    );

    const float ExtraHeight = FMath::Lerp(
        MinLockedCameraHeightOffset,
        MaxLockedCameraHeightOffset,
        HeightAlpha
    );

    FVector DesiredSocketOffset = SavedCameraSocketOffset;
    DesiredSocketOffset.Z += ExtraHeight;

    FVector CurrentSocketOffset = GameplayCameraBoom->SocketOffset;
    CurrentSocketOffset.Z = FMath::FInterpTo(
        CurrentSocketOffset.Z,
        DesiredSocketOffset.Z,
        DeltaTime,
        CameraHeightInterpSpeed
    );

    // Preserve any existing X/Y shoulder offset from the character Blueprint.
    CurrentSocketOffset.X = SavedCameraSocketOffset.X;
    CurrentSocketOffset.Y = SavedCameraSocketOffset.Y;
    GameplayCameraBoom->SocketOffset = CurrentSocketOffset;
}

void UTargetLockComponent::UpdateLockedCameraOffsets(float DeltaTime)
{
    if (!bAllowManualCameraWhileLocked || !bAutoRecenterLockedCamera || !GetWorld())
    {
        return;
    }

    const float TimeSinceInput = GetWorld()->GetTimeSeconds() - LastLockedCameraInputTime;
    if (TimeSinceInput < LockedCameraRecenterDelay)
    {
        return;
    }

    LockedCameraYawOffset = FMath::FInterpTo(
        LockedCameraYawOffset,
        0.0f,
        DeltaTime,
        LockedCameraRecenterSpeed
    );

    LockedCameraPitchOffset = FMath::FInterpTo(
        LockedCameraPitchOffset,
        0.0f,
        DeltaTime,
        LockedCameraRecenterSpeed
    );
}

void UTargetLockComponent::ApplyLockedCameraSettings()
{
    if (!bAllowManualCameraWhileLocked || !GameplayCameraBoom || !bSavedCameraSettings)
    {
        return;
    }

    // Decouple the camera boom from controller rotation. ControlRotation stays
    // target-facing for movement, while the spring arm gets a small bounded
    // manual yaw/pitch offset.
    GameplayCameraBoom->bUsePawnControlRotation = false;
}

void UTargetLockComponent::SaveCameraSettings()
{
    RefreshOwnerReferences();

    if (!GameplayCameraBoom || bSavedCameraSettings)
    {
        return;
    }

    SavedCameraArmLength = GameplayCameraBoom->TargetArmLength;
    SavedCameraSocketOffset = GameplayCameraBoom->SocketOffset;
    SavedCameraBoomRelativeRotation = GameplayCameraBoom->GetRelativeRotation();
    bSavedCameraBoomUsePawnControlRotation = GameplayCameraBoom->bUsePawnControlRotation;
    bSavedCameraSettings = true;
}

void UTargetLockComponent::RestoreCameraSettings()
{
    if (!GameplayCameraBoom || !bSavedCameraSettings)
    {
        return;
    }

    GameplayCameraBoom->TargetArmLength = SavedCameraArmLength;
    GameplayCameraBoom->SocketOffset = SavedCameraSocketOffset;
    GameplayCameraBoom->SetRelativeRotation(SavedCameraBoomRelativeRotation);
    GameplayCameraBoom->bUsePawnControlRotation = bSavedCameraBoomUsePawnControlRotation;
    bSavedCameraSettings = false;
}

void UTargetLockComponent::ApplyLockedMovementSettings()
{
    if (!OwnerCharacter)
    {
        return;
    }

    UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement();
    if (!Movement)
    {
        return;
    }

    if (!bSavedRotationSettings)
    {
        bSavedUseControllerRotationYaw = OwnerCharacter->bUseControllerRotationYaw;
        bSavedOrientRotationToMovement = Movement->bOrientRotationToMovement;
        bSavedUseControllerDesiredRotation = Movement->bUseControllerDesiredRotation;
        bSavedRotationSettings = true;
    }

    OwnerCharacter->bUseControllerRotationYaw = true;
    Movement->bOrientRotationToMovement = false;
    Movement->bUseControllerDesiredRotation = false;
}

void UTargetLockComponent::RestoreMovementSettings()
{
    if (!OwnerCharacter || !bSavedRotationSettings)
    {
        return;
    }

    if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
    {
        OwnerCharacter->bUseControllerRotationYaw = bSavedUseControllerRotationYaw;
        Movement->bOrientRotationToMovement = bSavedOrientRotationToMovement;
        Movement->bUseControllerDesiredRotation = bSavedUseControllerDesiredRotation;
    }

    bSavedRotationSettings = false;
}

void UTargetLockComponent::SwitchTarget(float Direction)
{
    if (!IsTargetLocked() || FMath::IsNearlyZero(Direction) || !OwnerPlayerController)
    {
        return;
    }

    FVector2D CurrentScreenPosition;
    if (!ProjectTargetToScreen(CurrentTarget, CurrentScreenPosition))
    {
        return;
    }

    TArray<AActor*> Candidates;
    GatherCandidateTargets(Candidates);

    AActor* BestTarget = nullptr;
    float BestSwitchScore = TNumericLimits<float>::Max();

    for (AActor* Candidate : Candidates)
    {
        if (Candidate == CurrentTarget || !IsTargetEligible(Candidate, false))
        {
            continue;
        }

        FVector2D CandidateScreenPosition;
        if (!ProjectTargetToScreen(Candidate, CandidateScreenPosition))
        {
            continue;
        }

        const float HorizontalDelta = CandidateScreenPosition.X - CurrentScreenPosition.X;

        if ((Direction < 0.0f && HorizontalDelta >= -1.0f)
            || (Direction > 0.0f && HorizontalDelta <= 1.0f))
        {
            continue;
        }

        const float VerticalDelta = FMath::Abs(
            CandidateScreenPosition.Y - CurrentScreenPosition.Y
        );

        // Prefer the closest neighboring target horizontally, with a smaller
        // penalty for large vertical jumps on screen.
        const float SwitchScore = FMath::Abs(HorizontalDelta) + (VerticalDelta * 0.35f);

        if (SwitchScore < BestSwitchScore)
        {
            BestSwitchScore = SwitchScore;
            BestTarget = Candidate;
        }
    }

    if (BestTarget)
    {
        LockTarget(BestTarget);
    }
}
