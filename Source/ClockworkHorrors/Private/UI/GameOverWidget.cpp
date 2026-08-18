// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/GameOverWidget.h"
#include "UI/ButtonWithText.h"
#include "CodeGameInstance.h"
#include "Interfaces/PlayerInterface.h"

void UGameOverWidget::NativeConstruct()
{
	if (Restart_Button) {
		Restart_Button->InternalButtonClicked.AddDynamic(this, &UGameOverWidget::RestartLevel);
	}
	if (Respawn_Button) {
		Respawn_Button->InternalButtonClicked.AddDynamic(this, &UGameOverWidget::Respawn);
	}
	if (MainMenu_Button) {
		MainMenu_Button->InternalButtonClicked.AddDynamic(this, &UGameOverWidget::ReturnToMainMenu);
	}
}

void UGameOverWidget::RestartLevel()
{
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadCurrentLevel();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is not of type UCodeGameInstance"));
	}
}

void UGameOverWidget::Respawn()
{
	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerPawn());
	if (PlayerInterface)
	{
		PlayerInterface->Respawn();
		RemoveFromParent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning player pawn does not implement IPlayerInterface"));
	}
}

void UGameOverWidget::ReturnToMainMenu()
{
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->LoadMainMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is not of type UCodeGameInstance"));
	}
}
