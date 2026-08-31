// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_TeleportToPlayer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_TeleportToPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	FVector PlayerForwardVector = Player->GetActorForwardVector();

	// Calculate position behind the player (200 units behind)
	float DistanceBehind = 500.0f;
	FVector TeleportLocation = PlayerLocation - (PlayerForwardVector * DistanceBehind);

	// Optional: Add slight height offset
	TeleportLocation.Z += 100.0f;

	AIPawn->SetActorLocation(TeleportLocation);

	return EBTNodeResult::Failed;
}
