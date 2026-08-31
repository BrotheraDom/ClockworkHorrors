#include "BTDecorator_CheckTargetHealth.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Utils/HealthComponent.h"

UBTDecorator_CheckTargetHealth::UBTDecorator_CheckTargetHealth()
{
	NodeName = "Check Target Health Threshold";
}

bool UBTDecorator_CheckTargetHealth::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!BBComp) return false;

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return false;

	// Find the HealthComponent attached to the Target Actor (Player)
	UHealthComponent* TargetHealthComp = TargetActor->FindComponentByClass<UHealthComponent>();
	if (!TargetHealthComp) return false;

	// Verify target can be healed (not dead and not at max health)
	if (!TargetHealthComp->CanHeal())
	{
		return false;
	}

	// Check if target's health ratio is strictly lower than our set threshold
	return TargetHealthComp->GetHealthPercent() < HealthThresholdPercent;
}