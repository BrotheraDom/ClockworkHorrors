// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Restart_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Respawn_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* MainMenu_Button;

	UFUNCTION()
	void RestartLevel();

	UFUNCTION()
	void Respawn();

	UFUNCTION()
	void ReturnToMainMenu();
	
};
