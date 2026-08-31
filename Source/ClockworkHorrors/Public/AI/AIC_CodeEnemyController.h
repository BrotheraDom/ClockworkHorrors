// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIC_CodeEnemyController.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API AAIC_CodeEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AAIC_CodeEnemyController();

	void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBehaviorTree* BehaviorTree;

private:
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
};
