// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/LavaDamageArea.h"
#include "Components/AudioComponent.h"
#include "CodeGameInstance.h" 

ALavaDamageArea::ALavaDamageArea()
{
	bDamagePlayer = true;
	bDamageOnEntry = true;
	bDamageOverTime = true;

	DamageAmount = 10.0f;
	DamageInterval = 1.0f;

	LavaSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LavaSound"));
	LavaSound->SetupAttachment(GetRootComponent());
}

void ALavaDamageArea::BeginPlay()
{
	ADamageAreaBase::BeginPlay();
	if (LavaSound)
	{
		LavaSound->Play();
		UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(GetGameInstance());
		if (GameInstance)
		{
			GameInstance->OnAudioChanged.AddDynamic(this, &ALavaDamageArea::AudioChanged);
			AudioChanged(GameInstance->GetMasterVolume(), GameInstance->GetSfxVolume(), GameInstance->GetMusicVolume());
		}
	}
}

void ALavaDamageArea::AudioChanged(float Master, float SFX, float Mus)
{
	if (LavaSound)
	{
		LavaSound->SetVolumeMultiplier(Master * SFX);
	}
}
