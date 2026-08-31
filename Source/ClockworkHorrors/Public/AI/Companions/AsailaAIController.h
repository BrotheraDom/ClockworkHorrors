// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AsailaAIController.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API AAsailaAIController : public AAIController
{
	GENERATED_BODY()

public:
	AAsailaAIController();

	void OnPossess(APawn* InPawn) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UBehaviorTree* BehaviorTree;

private:
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
};
