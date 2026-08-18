// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UOptionsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "Options Menu")
	void ShowOptionsMenu();

	UFUNCTION(BlueprintCallable, Category = "Options Menu")
	void HideOptionsMenu();


protected:
	
	UFUNCTION()
	void OnBackButtonClicked();

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnMasterVolumeChanged(float Value);

	UFUNCTION()
	void OnSfxVolumeChanged(float Value);

	UFUNCTION()
	void OnMusicVolumeChanged(float Value);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Back_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButtonWithText* Save_Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USlider* MasterVol_Slider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USlider* SFXVol_Slider;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class USlider* MusicVol_Slider;

	float MasterVolumetemp;
	float SfxVolumetemp;
	float MusicVolumetemp;
	
};
