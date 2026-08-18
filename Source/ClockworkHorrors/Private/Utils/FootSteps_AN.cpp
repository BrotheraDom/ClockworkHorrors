// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/FootSteps_AN.h"
#include "CodeGameInstance.h"

void UFootSteps_AN::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UE_LOG(LogTemp, Error, TEXT("Footstep notify triggered!"));
	UCodeGameInstance* GameInstance = Cast<UCodeGameInstance>(MeshComp->GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		float vol = GameInstance->GetSfxVolume() * GameInstance->GetMasterVolume();
		UE_LOG(LogTemp, Error, TEXT("Footstep notify triggered! SFX Volume: %f, Master Volume: %f, Final Volume: %f"), GameInstance->GetSfxVolume(), GameInstance->GetMasterVolume(), vol);
		VolumeMultiplier = vol;
	}

	Super::Notify(MeshComp, Animation, EventReference);
}
