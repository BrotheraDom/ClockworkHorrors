// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BTTask_Attack.h"
#include "AIController.h"
#include "Enemy.h"
#include "Interfaces/EnemyInterface.h"

UBTTask_Attack::UBTTask_Attack()
{
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: ExecuteTask called"));
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_Attack: No AI Controller"));
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_Attack: No Controlled Pawn"));
		return EBTNodeResult::Failed;
	}

	AEnemy* Character = Cast<AEnemy>(ControlledPawn);
	if (!Character)
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_Attack: Pawn is not a AEnemy"));
		return EBTNodeResult::Failed;
	}

	CachedOwnerComp = &OwnerComp;

	if (ControlledPawn->Implements<UEnemyInterface>())
	{
		OwnerComp.RegisterMessageObserver(this, "ActionFinished");
		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Registered as observer for message: %s"), TEXT("ActionFinished"));

		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Calling Attack on %s"), *ControlledPawn->GetName());
		IEnemyInterface* Interface = Cast<IEnemyInterface>(ControlledPawn);
		if (Interface)
		{
			Interface->PerformAttack();
			UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Attack executed on %s"), *ControlledPawn->GetName());
		}
		return EBTNodeResult::InProgress;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("BTTask_Attack: Pawn does not implement IEnemyInterface	"));
		CachedOwnerComp = nullptr;
		return EBTNodeResult::Failed;
	}
}

void UBTTask_Attack::OnMessage(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, FName Message, int32 SenderID, bool bSuccess)
{
	UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: OnMessage called - Message: %s"), *Message.ToString());

	if (Message == "ActionFinished")
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Received ActionFinished message, finishing task"));
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("BTTask_Attack: Received different message: %s, ignoring"), *Message.ToString());
	}
}
