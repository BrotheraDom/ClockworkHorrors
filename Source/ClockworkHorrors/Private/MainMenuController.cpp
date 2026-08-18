// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "MainMenuController.h"
#include "UI/MainMenuWidget.h"
#include "UI/OptionsMenuWidget.h"

void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	if (WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("WidgetClass is set to: %s"), *WidgetClass->GetName());
		MainMenuWidget = CreateWidget<UMainMenuWidget>(this, WidgetClass);

		UE_LOG(LogTemp, Warning, TEXT("Widget created: %s"), MainMenuWidget ? *MainMenuWidget->GetName() : TEXT("null"));
		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();

			FInputModeUIOnly InputMode;
			InputMode.SetWidgetToFocus(MainMenuWidget->TakeWidget());
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			SetInputMode(InputMode);
			bShowMouseCursor = true;
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to create widget! Please ensure that WidgetClass is a valid subclass of UMainMenuWidget."));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("WidgetClass is not set! Please assign a valid WidgetClass in the Blueprint or code."));
	}
}

void AMainMenuController::PreviousMenu()
{
	if (MainMenuWidget)
	{
		MainMenuWidget->PreviousMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MainMenuWidget is null. Cannot go to previous menu."));
	}
}
