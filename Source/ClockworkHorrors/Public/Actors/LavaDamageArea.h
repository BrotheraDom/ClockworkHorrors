// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/DamageAreaBase.h"
#include "LavaDamageArea.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API ALavaDamageArea : public ADamageAreaBase
{
	GENERATED_BODY()

public:
	ALavaDamageArea();

	void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	class UAudioComponent* LavaSound;

	UFUNCTION()
	void AudioChanged(float Master, float SFX, float Mus);
};
