// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTService_DistanceUpdate.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTService_DistanceUpdate::UBTService_DistanceUpdate()
{
	NodeName = "Update Distance From Player";

	Interval = 0.5f; 
	RandomDeviation = 0.1f; 

	DistanceKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DistanceUpdate, DistanceKey));
	PlayerActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_DistanceUpdate, PlayerActorKey), AActor::StaticClass());
}

void UBTService_DistanceUpdate::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		return;
	}
	AActor* PlayerActor = Cast<AActor>(BlackboardComp->GetValueAsObject(PlayerActorKey.SelectedKeyName));
	if (!PlayerActor)
	{
		return;
	}
	APawn* ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}
	float Distance = FVector::Dist(ControlledPawn->GetActorLocation(), PlayerActor->GetActorLocation());
	BlackboardComp->SetValueAsFloat(DistanceKey.SelectedKeyName, Distance);
}