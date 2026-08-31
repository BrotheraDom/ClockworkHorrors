// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/AIC_CodeEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/PlayerInterface.h"
#include "Interfaces/CompanionInterface.h"
AAIC_CodeEnemyController::AAIC_CodeEnemyController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	UAISenseConfig_Sight* SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 900.0f;
	SightConfig->LoseSightRadius = 1100.0f;
	SightConfig->PeripheralVisionAngleDegrees = 45.0f;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;



	PerceptionComponent->ConfigureSense(*SightConfig);

	SetPerceptionComponent(*PerceptionComponent);


	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_CodeEnemyController::OnPerceptionUpdated);
}

void AAIC_CodeEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Possessed pawn: %s"), *InPawn->GetName());
	if (PerceptionComponent)
	{
		PerceptionComponent->Activate(true);
	}
	RunBehaviorTree(BehaviorTree);
}

void AAIC_CodeEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();

	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("AIC_CodeEnemyController: Blackboard component is null. Behavior tree may not be running."));
		return;
	}
	if (Stimulus.WasSuccessfullySensed() && (Actor->Implements<UPlayerInterface>() || Actor->Implements<UCompanionInterface>()) && !BlackboardComp->GetValueAsObject("Target"))
	{
		UE_LOG(LogTemp, Warning, TEXT("AIC_CodeEnemyController: Player detected: %s"), *Actor->GetName());
		GetBlackboardComponent()->SetValueAsObject("Target", Actor);
	}
	else
	{
		AActor* CurrentTarget = Cast<AActor>(BlackboardComp->GetValueAsObject("Target"));
		if (CurrentTarget == Actor)
		{
			UE_LOG(LogTemp, Error, TEXT("AIC_CodeEnemyController: Player lost: %s"), *Actor->GetName());
			BlackboardComp->ClearValue("Target");
		}
	}
}
