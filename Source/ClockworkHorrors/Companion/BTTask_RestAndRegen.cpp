#include "BTTask_RestAndRegen.h"
#include "CompanionCharacter.h"
#include "CompanionAbilityComponent.h"
#include "AIController.h"

UBTTask_RestAndRegen::UBTTask_RestAndRegen()
{
	NodeName = "Rest And Regen Essence";
	bNotifyTaskFinished = true;
	bNotifyTick = true; // ?? Enable Ticking so the task can complete on its own
}

EBTNodeResult::Type UBTTask_RestAndRegen::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	IsEndingRest = false;

	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (!AIComp) return EBTNodeResult::Failed;

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
	if (!Companion) return EBTNodeResult::Failed;

	UCompanionAbilityComponent* AbilityComp = Companion->GetAbilityComponent();
	if (AbilityComp && AbilityComp->GetCurrentEssence() >= AbilityComp->GetMaxEssence())
	{
		return EBTNodeResult::Failed;
	}

	AIComp->StopMovement();
	if (AbilityComp)
	{
		AbilityComp->SetIsResting(true);
		AbilityComp->SetIsFullySeated(false); // Reset until AnimNotify fires
	}

	if (RestPoseMontage && Companion->GetMesh())
	{
		UAnimInstance* AnimInst = Companion->GetMesh()->GetAnimInstance();
		if (AnimInst && !AnimInst->Montage_IsPlaying(RestPoseMontage))
		{
			AnimInst->Montage_Play(RestPoseMontage, 1.0f);
			AnimInst->Montage_JumpToSection(FName("Start"), RestPoseMontage);
		}
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_RestAndRegen::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (!AIComp) return;

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
	if (!Companion) return;

	UCompanionAbilityComponent* AbilityComp = Companion->GetAbilityComponent();
	if (!AbilityComp) return;

	// 1. REGEN TICK: Increment Essence ONLY when fully seated in the Loop section
	if (AbilityComp->GetIsFullySeated() && !IsEndingRest)
	{
		if (AbilityComp->GetCurrentEssence() < AbilityComp->GetMaxEssence())
		{
			// 1.0f = total Essence restored per 1 second of sitting.
			// Adjust 1.0f higher/lower depending on how fast you want her bar to fill.
			float EssenceRatePerSecond = 1.0f;
			float EssenceToAddThisFrame = EssenceRatePerSecond * DeltaSeconds;

			AbilityComp->AddEssence(EssenceToAddThisFrame);
		}
	}

	// 2. ESSENCE FULL: Jump to End stand-up section
	if (AbilityComp->GetCurrentEssence() >= AbilityComp->GetMaxEssence() && !IsEndingRest)
	{
		IsEndingRest = true;

		if (RestPoseMontage && Companion->GetMesh() && Companion->GetMesh()->GetAnimInstance())
		{
			UAnimInstance* AnimInst = Companion->GetMesh()->GetAnimInstance();
			AnimInst->Montage_JumpToSection(FName("End"), RestPoseMontage);
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			return;
		}
	}

	// 3. STAND UP COMPLETE: Lock movement until End section finishes
	if (IsEndingRest)
	{
		AIComp->StopMovement();

		UAnimInstance* AnimInst = Companion->GetMesh()->GetAnimInstance();
		if (AnimInst && RestPoseMontage)
		{
			if (!AnimInst->Montage_IsPlaying(RestPoseMontage) || AnimInst->Montage_GetCurrentSection(RestPoseMontage) == FName("None"))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}

void UBTTask_RestAndRegen::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AAIController* AIComp = OwnerComp.GetAIOwner();
	if (AIComp)
	{
		ACompanionCharacter* Companion = Cast<ACompanionCharacter>(AIComp->GetPawn());
		if (Companion && Companion->GetAbilityComponent())
		{
			Companion->GetAbilityComponent()->SetIsResting(false);
			Companion->GetAbilityComponent()->SetIsFullySeated(false);

			if (TaskResult == EBTNodeResult::Aborted && RestPoseMontage && Companion->GetMesh() && Companion->GetMesh()->GetAnimInstance())
			{
				Companion->GetMesh()->GetAnimInstance()->Montage_Stop(0.15f, RestPoseMontage);
			}
		}
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}