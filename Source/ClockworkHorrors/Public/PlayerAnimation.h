// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerAnimation.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAnimDelegate);
UCLASS()
class CLOCKWORKHORRORS_API UPlayerAnimation : public UAnimInstance
{
	GENERATED_BODY()

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
protected:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Velocity;
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	float Direction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ActionSlotName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* swingingAsset;


public:
	void SwingingAnimation();
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FAnimDelegate OnAttackEnded;
};
