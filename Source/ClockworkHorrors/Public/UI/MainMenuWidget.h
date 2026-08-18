// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* PlayGame_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Options_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* QuitGame_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* ButtonPanel;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> OptionsMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	class UOptionsMenuWidget* OptionsMenuWidget;

	UFUNCTION()
	void OptionsButtonClicked();


public:
	UFUNCTION()
	void PreviousMenu();
};
