// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseSword.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API ABaseSword : public ABaseWeapon
{
	GENERATED_BODY()
	


protected:
	ABaseSword();
	virtual void BeginPlay() override;
	virtual void Attack() override;
	class UBoxComponent* HitBox;
	UFUNCTION()
	void BoundOverlap(UPrimitiveComponent* OverlappedComponenet, AActor* actor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
private:
	void ResetSwordHit();
};
