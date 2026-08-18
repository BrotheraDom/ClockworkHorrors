// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/OptionsSaveData.h"
#include "Kismet/GameplayStatics.h"

UOptionsSaveData::UOptionsSaveData()
{
	MasterVolume = 1.0f;
	SfxVolume = 1.0f;
	MusicVolume = 1.0f;
}

bool UOptionsSaveData::SaveOptionsData(float InMasterVolume, float InSfxVolume, float InMusicVolume)
{
	UOptionsSaveData* SaveData = NewObject<UOptionsSaveData>();

	if (!SaveData)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UOptionsSaveData object."));
		return false;
	}

	SaveData->MasterVolume = InMasterVolume;
	SaveData->SfxVolume = InSfxVolume;
	SaveData->MusicVolume = InMusicVolume;

	if (!UGameplayStatics::SaveGameToSlot(SaveData, TEXT("OptionsSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save options data to slot."));
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Options data saved successfully."));
	return true;
}

UOptionsSaveData* UOptionsSaveData::LoadOptionsData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("OptionsSaveSlot"), 0)) {
		UOptionsSaveData* LoadedData = Cast<UOptionsSaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("OptionsSaveSlot"), 0));

		if (LoadedData) {
			return LoadedData;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to load options data from slot."));
		}
		return nullptr;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No existing options save data found. Creating new save data."));
		UOptionsSaveData* NewSaveData = NewObject<UOptionsSaveData>();
		if (NewSaveData) {
			NewSaveData->SaveOptionsData(1.0f, 1.0f, 1.0f); 
			return NewSaveData;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to create new UOptionsSaveData object."));
		}
		return nullptr;
	}
}
