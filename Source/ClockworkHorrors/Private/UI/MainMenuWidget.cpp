// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/MainMenuWidget.h"
#include "UI/ButtonWithText.h"
#include "CodeGameInstance.h"
#include "UI/OptionsMenuWidget.h"
#include "Components/VerticalBox.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());

	if (GameInstance)
	{
		if (PlayGame_Button) {
			PlayGame_Button->InternalButtonClicked.AddDynamic(GameInstance, &UCodeGameInstance::LoadFirstLevel);
			UE_LOG(LogTemp, Warning, TEXT("PlayGame_Button is bound to LoadFirstLevel"));
		}
		if (QuitGame_Button) {
			QuitGame_Button->InternalButtonClicked.AddDynamic(GameInstance, &UCodeGameInstance::QuitTheGame);
		}
		if (Options_Button) {
			Options_Button->InternalButtonClicked.AddDynamic(this, &UMainMenuWidget::OptionsButtonClicked);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is not of type UCodeGameInstance"));
	}
}

void UMainMenuWidget::OptionsButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("OpenOptionsMenu called in UMainMenuWidget"));
	if (OptionsMenuWidgetClass)
	{
		OptionsMenuWidget = CreateWidget<UOptionsMenuWidget>(GetOwningPlayer(), OptionsMenuWidgetClass);
		if (OptionsMenuWidget)
		{
			OptionsMenuWidget->SetOwningPlayer(GetOwningPlayer());
			OptionsMenuWidget->ShowOptionsMenu();
			ButtonPanel->SetVisibility(ESlateVisibility::Hidden);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to create OptionsMenuWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("OptionsMenuWidgetClass is not set"));
	}
}

void UMainMenuWidget::PreviousMenu()
{
	ButtonPanel->SetVisibility(ESlateVisibility::Visible);
	OptionsMenuWidget->RemoveFromParent();
}
