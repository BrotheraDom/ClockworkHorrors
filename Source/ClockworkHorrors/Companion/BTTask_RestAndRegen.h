#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RestAndRegen.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API UBTTask_RestAndRegen : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RestAndRegen();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
protected:
	/** Animation Montage to play during rest/meditation */
	UPROPERTY(EditAnywhere, Category = "Animation")
	UAnimMontage* RestPoseMontage;

	/** Blend out time when stopping montage early (e.g. upon movement) */
	UPROPERTY(EditAnywhere, Category = "Animation")
	float MontageBlendOutTime = 0.25f;
private:
	bool IsEndingRest = false;
};