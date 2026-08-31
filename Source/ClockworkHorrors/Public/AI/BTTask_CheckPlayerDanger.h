// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CheckPlayerDanger.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UBTTask_CheckPlayerDanger : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_CheckPlayerDanger();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
