// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StatusEffectType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class STATUSEFFECT : uint8
{
	None,
	Stunned,
	Slowed,
	Weakened,
	Poisoned,
	Burning
};

UCLASS()
class CLOCKWORKHORRORS_API UStatusEffectType : public UDataAsset
{
	GENERATED_BODY()

	public:
		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status")
		STATUSEFFECT Effect = STATUSEFFECT::None;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.0"))
		float Duration = 0.0f;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.0"))
		float TickDamage = 0.0f;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.01"))
		float TickInterval = 1.0f;

		UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Status", meta = (ClampMin = "0.0", ClampMax = "1.0"))
		float Percentage = 0.0f;
	
};
