#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckTargetHealth.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API UBTDecorator_CheckTargetHealth : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CheckTargetHealth();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	/** Blackboard key selecting the Target Actor (Player) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Trigger healing when target health ratio is BELOW this threshold (e.g., 0.85 = 85% HP) */
	UPROPERTY(EditAnywhere, Category = "Health Check", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThresholdPercent = 0.85f;
};