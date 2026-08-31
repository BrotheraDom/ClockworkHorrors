// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_CastThornLashStrike.h"
#include "Interfaces/CompanionInterface.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_CastThornLashStrike::UBTTask_CastThornLashStrike()
{
}

EBTNodeResult::Type UBTTask_CastThornLashStrike::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: ExecuteTask called"));
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject("Target"));

	if (!TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: TargetActor is null"));
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: AIController is null"));
		return EBTNodeResult::Failed;
	}

	APawn* AIPawn = AIController->GetPawn();
	if (!AIPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: AIPawn is null"));
		return EBTNodeResult::Failed;
	}

	ICompanionInterface* CompanionInterface = Cast<ICompanionInterface>(AIPawn);

	if (!CompanionInterface)
	{
		return EBTNodeResult::Failed;
	}
	bool bCastSuccessful = CompanionInterface->CastSpellSlot(1, TargetActor);


	if (!bCastSuccessful)
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: CastSpellSlot failed"));
		return EBTNodeResult::Failed;
	}
	UE_LOG(LogTemp, Warning, TEXT("BTTask_CastThornLashStrike: CastSpellSlot succeeded"));
	return EBTNodeResult::Succeeded;
}

void UBTTask_CastThornLashStrike::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 SenderID, bool bSuccess)
{
}
