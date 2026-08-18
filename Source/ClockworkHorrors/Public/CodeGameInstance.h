// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodeGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UCodeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	virtual void Init() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	int FirstLevelIndex;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	TArray<FName> GameLevels;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	int CurrentLevel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float MasterVolume;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float SfxVolume;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float MusicVolume;

public:

	UFUNCTION()
	void LoadFirstLevel();

	UFUNCTION()
	void QuitTheGame();

	UFUNCTION()
	void LoadLevelSafe(int LevelIndex);

	UFUNCTION()
	void LoadCurrentLevel();

	UFUNCTION()
	void LoadMainMenu();

	UFUNCTION()
	void SetMasterVolume(float Value);
	UFUNCTION()
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION()
	void SetSfxVolume(float Value);
	UFUNCTION()
	float GetSfxVolume() const { return SfxVolume; }

	UFUNCTION()
	void SetMusicVolume(float Value);
	UFUNCTION()
	float GetMusicVolume() const { return MusicVolume; }

};
