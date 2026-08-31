// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_FollowPlayer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_FollowPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Player = Cast<APawn>(BlackboardComp->GetValueAsObject("Player"));

	if (!Player)
	{
		return EBTNodeResult::Failed;
	}

	FVector PlayerLocation = Player->GetActorLocation();

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	bool bSuccess = NavSystem->K2_GetRandomLocationInNavigableRadius(AIPawn, AIPawn->GetActorLocation(), PlayerLocation, Radius, nullptr, nullptr);

	if (bSuccess)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("Location", PlayerLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
