// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/MainMenuInterface.h"
#include "MainMenuController.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API AMainMenuController : public APlayerController, public IMainMenuInterface
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> WidgetClass;

	class UMainMenuWidget* MainMenuWidget;

	
	virtual void PreviousMenu() override;
};
