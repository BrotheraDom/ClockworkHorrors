// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OptionsSaveData.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UOptionsSaveData : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	float MasterVolume;

	UPROPERTY()
	float SfxVolume;

	UPROPERTY()
	float MusicVolume;


	UOptionsSaveData();

	bool SaveOptionsData(float InMasterVolume, float InSfxVolume, float InMusicVolume);

	static UOptionsSaveData* LoadOptionsData();
	
};
