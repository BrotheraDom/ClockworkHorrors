// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemActionWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FItemActionDelegate, int32, Index);

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UItemActionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UItemActionWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativePreConstruct() override;

	virtual void NativeConstruct() override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
	void ShowWidget();

	UFUNCTION()
	void ShowEquipAction();

	UFUNCTION()
	void ShowUseAction();

	UFUNCTION()
	void ShowDropAction();

	UFUNCTION()
	void HideWidget();

	UFUNCTION()
	void HideEquipAction();

	UFUNCTION()
	void HideUseAction();

	UFUNCTION()
	void HideDropAction();

	UFUNCTION()
	void ResetMenu();

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* EquipSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UButtonWithText* EquipButton;

	UPROPERTY(meta = (BindWidget))
	class UButtonWithText* UnequipButton;

	UPROPERTY(meta = (BindWidget))
	class UButtonWithText* UseButton;

	UPROPERTY(meta = (BindWidget))
	class UButtonWithText* DropButton;

	UPROPERTY()
	FItemActionDelegate OnItemActionClicked;

protected:
	UFUNCTION()
	void OnEquipActionButtonClicked();

	UFUNCTION()
	void OnUnequipActionButtonClicked();

	UFUNCTION()
	void OnUseActionButtonClicked();

	UFUNCTION()
	void OnDropActionButtonClicked();
	
};
