// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CodeGameInstance.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDelegateAudioChanged, float, MasterVolume, float, SfxVolume, float, MusicVolume);

UCLASS()
class CLOCKWORKHORRORS_API UCodeGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	virtual void Init() override;

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	int FirstLevelIndex;



	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	int CurrentLevel;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float MasterVolume;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float SfxVolume;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	float MusicVolume;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Variables")
	TArray<FName> GameLevels;
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Variables")
	FDelegateAudioChanged OnAudioChanged;

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
	UFUNCTION(BlueprintCallable)
	float GetMasterVolume() const { return MasterVolume; }

	UFUNCTION()
	void SetSfxVolume(float Value);
	UFUNCTION(BlueprintCallable)
	float GetSfxVolume() const { return SfxVolume; }

	UFUNCTION()
	void SetMusicVolume(float Value);
	UFUNCTION(BlueprintCallable)
	float GetMusicVolume() const { return MusicVolume; }

};
