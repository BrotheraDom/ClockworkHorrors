#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CanRest.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API UBTDecorator_CanRest : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CanRest();

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	/** Maximum speed (cm/s) allowed to be considered "stationary" */
	UPROPERTY(EditAnywhere, Category = "Condition")
	float MaxStationarySpeed = 10.0f;
};