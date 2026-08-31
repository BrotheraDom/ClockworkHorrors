#include "UBTService_UpdateCompanionSpeed.h" // <-- MUST BE FIRST!

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_UpdateCompanionSpeed::UBTService_UpdateCompanionSpeed()
{
	NodeName = "Update Companion Speed";

	// Default tick interval for performance (checks every 0.1s instead of every frame)
	Interval = 0.1f;
	RandomDeviation = 0.02f;

	// Automatically filter Blackboard selectors to only show Actor/Object keys in the editor
	TargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_UpdateCompanionSpeed, TargetActorKey), AActor::StaticClass());
}

void UBTService_UpdateCompanionSpeed::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp) return;

	ACharacter* CompanionChar = Cast<ACharacter>(AIController->GetPawn());
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!CompanionChar || !TargetActor) return;

	UCharacterMovementComponent* MoveComp = CompanionChar->GetCharacterMovement();
	if (!MoveComp) return;

	// Calculate 2D distance (ignoring vertical height differences)
	const float Distance = FVector::Dist2D(CompanionChar->GetActorLocation(), TargetActor->GetActorLocation());

	// Map distance smoothly between WalkSpeed and RunSpeed
	if (Distance <= WalkDistanceThreshold)
	{
		MoveComp->MaxWalkSpeed = WalkSpeed;
	}
	else if (Distance >= RunDistanceThreshold)
	{
		MoveComp->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		// Smooth linear interpolation between walk distance and run distance
		const float Alpha = (Distance - WalkDistanceThreshold) / (RunDistanceThreshold - WalkDistanceThreshold);
		MoveComp->MaxWalkSpeed = FMath::Lerp(WalkSpeed, RunSpeed, Alpha);
	}
}