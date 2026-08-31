#include "CompanionAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ACompanionAIController::ACompanionAIController()
{
    PrimaryActorTick.bCanEverTick = false;
}

void ACompanionAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    if (BehaviorTreeAsset)
    {
        // 1. Run the Behavior Tree
        RunBehaviorTree(BehaviorTreeAsset);

        // 2. Assign the player target on the next tick (handles runtime spawning cleanly)
        GetWorldTimerManager().SetTimerForNextTick([this]()
            {
                if (ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
                {
                    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
                    {
                        BBComp->SetValueAsObject(TEXT("TargetActor"), PlayerCharacter);
                    }
                }
            });
    }
}