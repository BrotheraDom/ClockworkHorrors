// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PlayerInterface.h"
#include "Components/CharacterAnimationComponent.h"
#include "BaseCharacter.generated.h"

class SWidget;
class USkeletalMeshComponent;
class ARiflePickup;
class UHealthComponent;
class UInventoryComponent;
class ABaseWeapon;
class UUserWidget;
class UPauseMenuWidget;
class UGameOverWidget;
class UOptionsMenuWidget;
class UMainInventoryWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDelegate);

UCLASS()
class CLOCKWORKHORRORS_API ABaseCharacter : public ACharacter, public IPlayerInterface
{
    GENERATED_BODY()

public:
    ABaseCharacter();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintCallable, Category = "UI")
    void HandlePausePressed();

    UFUNCTION(BlueprintCallable, Category = "UI")
    void QuitGameFromPause();

    UFUNCTION(BlueprintCallable, Category = "Interact")
    void TryPickupInteract();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void EquipPickupWeapon();

    UFUNCTION(BlueprintCallable, Category = "Weapon")
    void Attack();

    // Kept for compatibility with existing weapon code such as BaseSword,
    // which broadcasts this delegate when an attack begins.
    UPROPERTY(BlueprintAssignable, Category = "Weapon")
    FDelegate OnAttack;

    void Reload();

    UFUNCTION()
    void HandleDeath();

    UFUNCTION()
    void InventoryHUD();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    ABaseWeapon* weapon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
    bool bHasWeapon = false;

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    bool bIsDead = false;

    void ShowGameOverOverlay();
    void HideGameOverOverlay();
    void ShowPauseOverlay();
    void HidePauseOverlay();
    void ShowGameOverMenu();
    void RestartLevel();
    void ShowControlsOverlay();
    void HideControlsOverlay();
    void ShowStartScreen();
    void HideStartScreen();
    void ShowHealthBarHUD();
    void HideHealthBarHUD();

    float GetHealthPercent() const;
    FText GetHealthText() const;

    TSubclassOf<AActor> weaponClass;

    UPROPERTY(Transient, BlueprintReadOnly, Category = "Animation")
    TObjectPtr<UCharacterAnimationComponent> CharacterAnimationComponent;

    // Components
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    UHealthComponent* HealthComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
    UInventoryComponent* InventoryComponent;

    UPROPERTY(BlueprintReadOnly, Category = "Checkpoint Manager")
    TObjectPtr<UObject> CheckpointManager;

    UFUNCTION(BlueprintImplementableEvent, Category = "Health")
    void OnDeathBP();

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> PauseMenuClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UPauseMenuWidget* PauseMenuWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> GameOverMenuClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UGameOverWidget* GameOverMenuWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> OptionsMenuClass;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UOptionsMenuWidget* OptionsMenuWidget;

    UPROPERTY(BlueprintReadOnly, Category = "UI")
    UMainInventoryWidget* InventoryWidget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    TSubclassOf<UUserWidget> InventoryHudClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
    bool isInInventory;

private:
    TSharedPtr<SWidget> GameOverSlateWidget;
    bool bGameOverOverlayShown = false;

    TSharedPtr<SWidget> PauseSlateWidget;
    bool bPauseOverlayShown = false;

    TSharedPtr<SWidget> ControlsSlateWidget;
    bool bControlsOverlayShown = false;

    TSharedPtr<SWidget> StartSlateWidget;
    bool bStartScreenShown = false;

    TSharedPtr<SWidget> HealthBarSlateWidget;
    bool bHealthBarShown = false;

public:
    UFUNCTION()
    void AttackingAnim();

    UFUNCTION()
    void AttackAnimEnded();

    UFUNCTION()
    void HandleActionAnimationEnded(
        ECharacterActionAnimationType AnimationType,
        bool bInterrupted
    );

    virtual void ResumeGame() override;
    virtual void OpenOptionsMenu() override;
    virtual void PreviousMenu() override;
    virtual void Respawn() override;
};