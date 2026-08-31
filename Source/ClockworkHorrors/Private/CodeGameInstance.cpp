// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "CodeGameInstance.h"
#include "UI/OptionsMenuWidget.h"
#include "Utils/OptionsSaveData.h"
#include "Utils/PlayerSaveGame.h"
#include "BaseCharacter.h"


void UCodeGameInstance::Init()
{
	//GameLevels = { "MainMenu", "Level1TestingMap" };
	UOptionsSaveData* OptionsData = UOptionsSaveData::LoadOptionsData();
	if (!OptionsData) {
		UE_LOG(LogTemp, Warning, TEXT("Failed to load options data. Using default values."));
		OptionsData = NewObject<UOptionsSaveData>();
	}
	MasterVolume = OptionsData->MasterVolume;
	SfxVolume = OptionsData->SfxVolume;
	MusicVolume = OptionsData->MusicVolume;
	UPlayerSaveGame* playerSave = UPlayerSaveGame::LoadPlayerData();
}

void UCodeGameInstance::LoadFirstLevel()
{
	UE_LOG(LogTemp, Warning, TEXT("LoadFirstLevel called. FirstLevelIndex is: %d"), FirstLevelIndex);
	LoadLevelSafe(FirstLevelIndex);
}

void UCodeGameInstance::QuitTheGame()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (PlayerController)
	{
		PlayerController->ConsoleCommand("quit");
	}
}

void UCodeGameInstance::LoadLevelSafe(int LevelIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Attempting to load level with index: %d"), LevelIndex);
	if (GameLevels.IsValidIndex(LevelIndex))
	{
		FName LevelName = GameLevels[LevelIndex];
		CurrentLevel = LevelIndex;

		UE_LOG(LogTemp, Warning, TEXT("Loading level: %s"), *LevelName.ToString());

		UWorld* World = GetWorld();
		if (World == nullptr)
		{
			return;
		}
			APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
			if (PlayerController) {
				ABaseCharacter* player = Cast<ABaseCharacter>(PlayerController->GetPawn());
				if (player)
				{
					player->OnLevelChange.Broadcast();
				}
					
				PlayerController->ClientTravel(LevelName.ToString(), ETravelType::TRAVEL_Absolute);
			}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid level index: %d"), LevelIndex);
	}
}

void UCodeGameInstance::LoadCurrentLevel()
{
	LoadLevelSafe(CurrentLevel);
}

void UCodeGameInstance::LoadMainMenu()
{
	LoadLevelSafe(0);
}

void UCodeGameInstance::SetMasterVolume(float Value)
{
	MasterVolume = Value;
	OnAudioChanged.Broadcast(MasterVolume, SfxVolume, MusicVolume);
}

void UCodeGameInstance::SetSfxVolume(float Value)
{
	SfxVolume = Value;
	OnAudioChanged.Broadcast(MasterVolume, SfxVolume, MusicVolume);
}

void UCodeGameInstance::SetMusicVolume(float Value)
{
	MusicVolume = Value;
	OnAudioChanged.Broadcast(MasterVolume, SfxVolume, MusicVolume);
}

