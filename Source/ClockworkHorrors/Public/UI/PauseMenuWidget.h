// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Resume_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Options_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Controls_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* MainMenu_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UVerticalBox* ButtonPanel;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UBorder* ControlsBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* ControlsBack_Button;


	private:
		UFUNCTION()
		void ResumeGame();

		UFUNCTION()
		void OpenOptionsMenu();

		UFUNCTION()
		void OpenControlsMenu();

		UFUNCTION()
		void ReturnToMainMenu();

		UFUNCTION()
		void CloseControlsMenu();




};
