// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/OptionsMenuWidget.h"
#include "UI/ButtonWithText.h"
#include "CodeGameInstance.h"
#include "Interfaces/PlayerInterface.h"
#include "Interfaces/MainMenuInterface.h"
#include "Components/Slider.h"
#include "Utils/OptionsSaveData.h"




void UOptionsMenuWidget::NativeConstruct()
{
	if (Back_Button)
	{
		Back_Button->InternalButtonClicked.AddDynamic(this, &UOptionsMenuWidget::OnBackButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Back_Button is not bound"));
	}

	if (MasterVol_Slider)
	{
		MasterVol_Slider->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnMasterVolumeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MasterVol_Slider is not bound"));
	}
	if (SFXVol_Slider)
	{
		SFXVol_Slider->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnSfxVolumeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SFXVol_Slider is not bound"));
	}
	if (MusicVol_Slider)
	{
		MusicVol_Slider->OnValueChanged.AddDynamic(this, &UOptionsMenuWidget::OnMusicVolumeChanged);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("MusicVol_Slider is not bound"));
	}

	if (Save_Button)
	{
		Save_Button->InternalButtonClicked.AddDynamic(this, &UOptionsMenuWidget::OnSaveButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Save_Button is not bound"));
	}
}

void UOptionsMenuWidget::ShowOptionsMenu()
{
	UE_LOG(LogTemp, Warning, TEXT("ShowOptionsMenu called"));
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		MasterVol_Slider->SetValue(GameInstance->GetMasterVolume());
		SFXVol_Slider->SetValue(GameInstance->GetSfxVolume());
		MusicVol_Slider->SetValue(GameInstance->GetMusicVolume());
		UE_LOG(LogTemp, Warning, TEXT("ShowOptionsMenu - Master: %f, SFX: %f, Music: %f"),GameInstance->GetMasterVolume(), GameInstance->GetSfxVolume(), GameInstance->GetMusicVolume());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance is not of type UCodeGameInstance"));
	}
	
	MasterVolumetemp = MasterVol_Slider->GetValue();
	SfxVolumetemp = SFXVol_Slider->GetValue();
	MusicVolumetemp = MusicVol_Slider->GetValue();

	AddToViewport(1);
	SetVisibility(ESlateVisibility::Visible);

	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		PlayerController->SetInputMode(InputMode);
		PlayerController->bShowMouseCursor = true;
	}
}

void UOptionsMenuWidget::HideOptionsMenu()
{
	RemoveFromParent();
}

void UOptionsMenuWidget::OnBackButtonClicked()
{
	HideOptionsMenu();
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance) {
		GameInstance->SetMasterVolume(MasterVolumetemp);
		GameInstance->SetSfxVolume(SfxVolumetemp);
		GameInstance->SetMusicVolume(MusicVolumetemp);
	}
	
	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(GetOwningPlayerPawn());
	if (PlayerInterface)
	{
		PlayerInterface->PreviousMenu();
		return;
	}
	
	IMainMenuInterface* MenuInterface = Cast<IMainMenuInterface>(GetOwningPlayer());

	if (MenuInterface)
	{
		MenuInterface->PreviousMenu();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Owning player pawn does not implement IPlayerInterface or IMainMenuInterface"));
	}

}

void UOptionsMenuWidget::OnMasterVolumeChanged(float Value)
{
	//UE_LOG(LogTemp, Log, TEXT("Master Volume Changed: %f"), Value);
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetMasterVolume(Value);
	}
}

void UOptionsMenuWidget::OnSfxVolumeChanged(float Value)
{
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetSfxVolume(Value);
	}
}

void UOptionsMenuWidget::OnMusicVolumeChanged(float Value)
{
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
	if (GameInstance)
	{
		GameInstance->SetMusicVolume(Value);
	}
}

void UOptionsMenuWidget::OnSaveButtonClicked()
{
	UOptionsSaveData* SaveData = NewObject<UOptionsSaveData>();
	if (SaveData)
	{
		SaveData->SaveOptionsData(MasterVol_Slider->GetValue(), SFXVol_Slider->GetValue(), MusicVol_Slider->GetValue());
		MasterVolumetemp = MasterVol_Slider->GetValue();
		SfxVolumetemp = SFXVol_Slider->GetValue();
		MusicVolumetemp = MusicVol_Slider->GetValue();
	}
}
