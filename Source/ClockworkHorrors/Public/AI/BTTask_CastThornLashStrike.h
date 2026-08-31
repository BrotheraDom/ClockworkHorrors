// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_CastThornLashStrike.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UBTTask_CastThornLashStrike : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_CastThornLashStrike();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 SenderID, bool bSuccess) override;
};
