// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/Companions/AsailaAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

AAsailaAIController::AAsailaAIController()
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
	PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAsailaAIController::OnPerceptionUpdated);
}

void AAsailaAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	UE_LOG(LogTemp, Warning, TEXT("Possessed pawn: %s"), *InPawn->GetName());
	if (PerceptionComponent)
	{
		PerceptionComponent->Activate(true);
	}
	RunBehaviorTree(BehaviorTree);
	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (BlackboardComp)
	{
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			BlackboardComp->SetValueAsObject("Player", PlayerPawn);
			UE_LOG(LogTemp, Warning, TEXT("AsailaAIController: Player set in blackboard: %s"), *PlayerPawn->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("AsailaAIController: Failed to get player pawn."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AsailaAIController: Blackboard component is null. Behavior tree may not be running."));
	}
}

void AAsailaAIController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	/*UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("AsailaAIController: Blackboard component is null. Behavior tree may not be running."));
		return;
	}
	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOG(LogTemp, Warning, TEXT("AsailaAIController: Player detected: %s"), *Actor->GetName());
		GetBlackboardComponent()->SetValueAsObject("Target", Actor);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AsailaAIController: Player lost: %s"), *Actor->GetName());
		GetBlackboardComponent()->ClearValue("Target");
	}*/
} 
