#include "BTTask_HealingTouch.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "CompanionCharacter.h"
#include "CompanionAbilityComponent.h"

UBTTask_HealingTouch::UBTTask_HealingTouch()
{
	bNotifyTick = true;
	NodeName = "Perform Healing Touch";
}

EBTNodeResult::Type UBTTask_HealingTouch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bIsCasting = false; // Reset state at start of execution

	AAIController* AIComp = OwnerComp.GetAIOwner();
	UBlackboardComponent* BBComp = OwnerComp.GetBlackboardComponent();
	if (!AIComp || !BBComp) return EBTNodeResult::Failed;

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
	if (!Companion) return EBTNodeResult::Failed;

	AActor* TargetActor = Cast<AActor>(BBComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!TargetActor) return EBTNodeResult::Failed;

	UCompanionAbilityComponent* AbilityComp = Companion->GetAbilityComponent();

	// HARD GUARD: Ensure essence is strictly >= cost
	if (!AbilityComp || AbilityComp->GetCurrentEssence() < AbilityComp->HealingTouchCost)
	{
		AIComp->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Failed;
	}

	// Turn toward target before casting
	AIComp->SetFocus(TargetActor);
	AIComp->StopMovement();

	// Trigger ability execution and animation
	bool bSuccess = AbilityComp->PerformHealingTouch(TargetActor);
	if (!bSuccess)
	{
		AIComp->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Failed;
	}

	bIsCasting = true; // FIX 1: Set flag so TickTask evaluates!
	return EBTNodeResult::InProgress;
}

void UBTTask_HealingTouch::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsCasting) return;

	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (!AIComp)
	{
		bIsCasting = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
	if (!Companion || !Companion->GetMesh())
	{
		bIsCasting = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UCompanionAbilityComponent* AbilityComp = Companion->GetAbilityComponent();
	if (!AbilityComp)
	{
		bIsCasting = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UAnimInstance* AnimInst = Companion->GetMesh()->GetAnimInstance();
	const bool bMontageFinished = !AnimInst || !AnimInst->Montage_IsPlaying(AbilityComp->CastHealMontage);
	const bool bOrbFinished = (AbilityComp->GetCurrentOrbState() == EOrbFlightState::Orbiting);

	// FIX 2: Complete task when montage completes AND orb returns to Orbiting state
	if (bMontageFinished && bOrbFinished)
	{
		bIsCasting = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_HealingTouch::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	bIsCasting = false; // Reset on task end or abort

	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (AIComp)
	{
		AIComp->ClearFocus(EAIFocusPriority::Gameplay);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}