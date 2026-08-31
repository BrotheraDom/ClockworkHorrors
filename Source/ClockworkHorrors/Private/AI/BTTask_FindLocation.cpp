// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_FindLocation.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindLocation::UBTTask_FindLocation()
{
}

EBTNodeResult::Type UBTTask_FindLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(AIPawn->GetWorld());
	FVector RandomLocation;
	bool bSuccess = NavSystem->K2_GetRandomLocationInNavigableRadius(AIPawn, AIPawn->GetActorLocation(), RandomLocation, Radius, nullptr, nullptr);

	if (bSuccess)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("Location", RandomLocation);
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
