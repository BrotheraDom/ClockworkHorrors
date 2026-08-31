// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_CastHealingTouch.h"
#include "Interfaces/CompanionInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastHealingTouch::UBTTask_CastHealingTouch()
{
}

EBTNodeResult::Type UBTTask_CastHealingTouch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* PlayerActor = Cast<AActor>(BlackboardComp->GetValueAsObject("Player"));

	if (!PlayerActor)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		return EBTNodeResult::Failed;
	}

	ICompanionInterface* CompanionInterface = Cast<ICompanionInterface>(AIPawn);

	if (!CompanionInterface)
	{
		return EBTNodeResult::Failed;
	}
	bool bCastSuccessful = CompanionInterface->CastSpellSlot(0, PlayerActor); 
	

	if (!bCastSuccessful)
	{
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Succeeded;
}

void UBTTask_CastHealingTouch::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 SenderID, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: OnMessage called - Message: %s"), *Message.ToString());

	if (Message == "ActionFinished")
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Received ActionFinished message, finishing task"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Received different message: %s, ignoring"), *Message.ToString());
	}
}
