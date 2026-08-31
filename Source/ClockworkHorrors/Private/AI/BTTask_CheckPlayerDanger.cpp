// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_CheckPlayerDanger.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/PlayerInterface.h"

UBTTask_CheckPlayerDanger::UBTTask_CheckPlayerDanger()
{
}

EBTNodeResult::Type UBTTask_CheckPlayerDanger::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	IPlayerInterface* PlayerInterface = Cast<IPlayerInterface>(BlackboardComp->GetValueAsObject("Player"));

	if (!PlayerInterface)
	{
		return EBTNodeResult::Failed;
	}
	else {
		float PlayerHealth = PlayerInterface->GetCurrentHealth();
		if (PlayerHealth <= .5f) 
		{
			UE_LOG(LogTemp, Warning, TEXT("BTTask_CheckPlayerDanger: Player is in danger! Health: %f"), PlayerHealth);
			return EBTNodeResult::Succeeded;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
}