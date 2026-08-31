// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindLocation.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UBTTask_FindLocation : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_FindLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY()
	float Radius = 1000.0f;
};
