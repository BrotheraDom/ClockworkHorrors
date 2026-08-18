// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/PauseMenuWidget.h"
#include "CodeGameInstance.h"
#include "Components/VerticalBox.h"
#include "UI/ButtonWithText.h"
#include "Interfaces/PlayerInterface.h"
#include "Components/Border.h"

void UPauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
	//UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (Resume_Button) {
		Resume_Button->InternalButtonClicked.AddDynamic(this, &UPauseMenuWidget::ResumeGame);
	}
	if (Options_Button) {
		Options_Button->InternalButtonClicked.AddDynamic(this, &UPauseMenuWidget::OpenOptionsMenu);
	}
	if (Controls_Button) {
		Controls_Button->InternalButtonClicked.AddDynamic(this, &UPauseMenuWidget::OpenControlsMenu);
	}
	if (MainMenu_Button) {
		MainMenu_Button->InternalButtonClicked.AddDynamic(this, &UPauseMenuWidget::ReturnToMainMenu);
	}
	if (ControlsBorder) {
		ControlsBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	if (ControlsBack_Button) {
		ControlsBack_Button->InternalButtonClicked.AddDynamic(this, &UPauseMenuWidget::CloseControlsMenu);
	}
}

void UPauseMenuWidget::ResumeGame()
{
	//UE_LOG(LogTemp, Warning, TEXT("Resuming the game..."));
	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerPawn());
	if (PlayerInterface)
	{
		PlayerInterface->ResumeGame();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning player pawn does not implement IPlayerInterface"));
	}
}

void UPauseMenuWidget::OpenOptionsMenu()
{
	//UE_LOG(LogTemp, Warning, TEXT("Opening Options Menu..."));
//	ButtonPanel->SetVisibility(ESlateVisibility::Hidden);
	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerPawn());
	if (PlayerInterface)
	{
		PlayerInterface->OpenOptionsMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning player pawn does not implement IPlayerInterface"));
	}
}

void UPauseMenuWidget::OpenControlsMenu()
{
	//UE_LOG(LogTemp, Warning, TEXT("Opening Controls Menu..."));
	ButtonPanel->SetVisibility(ESlateVisibility::Hidden);
	ControlsBorder->SetVisibility(ESlateVisibility::Visible);
}

void UPauseMenuWidget::ReturnToMainMenu()
{
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadLevelSafe(0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is not of type UCodeGameInstance"));
	}
}

void UPauseMenuWidget::CloseControlsMenu()
{
	ControlsBorder->SetVisibility(ESlateVisibility::Hidden);
	ButtonPanel->SetVisibility(ESlateVisibility::Visible);
}
