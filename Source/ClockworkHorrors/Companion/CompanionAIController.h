#pragma once

#include "CoreMinimal.h"
#include "DetourCrowdAIController.h"
#include "CompanionAIController.generated.h"

class UBehaviorTree;

UCLASS()
class CLOCKWORKHORRORS_API ACompanionAIController : public ADetourCrowdAIController
{
    GENERATED_BODY()

public:
    ACompanionAIController();

protected:
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(EditDefaultsOnly, Category = "AI")
    UBehaviorTree* BehaviorTreeAsset;
};