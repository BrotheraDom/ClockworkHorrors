#include "BTDecorator_CanRest.h"
#include "CompanionCharacter.h"
#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTDecorator_CanRest::UBTDecorator_CanRest()
{
	NodeName = "Can Rest (Stationary & Out of Combat)";
}

bool UBTDecorator_CanRest::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (!AIComp) return false;

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
	if (!Companion) return false;

	// 1. Check Movement Speed (Is Stationary)
	const float CurrentSpeed = Companion->GetVelocity().Size2D();
	if (CurrentSpeed > MaxStationarySpeed)
	{
		return false;
	}

	// 2. Check Combat State (Expandable when you add a combat state flag to Asaila)
	// For now, if speed is 0 and she isn't actively pathfinding, she can rest:
	return true;
}