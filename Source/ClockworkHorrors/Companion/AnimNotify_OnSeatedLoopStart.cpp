#include "AnimNotify_OnSeatedLoopStart.h"
#include "CompanionCharacter.h"
#include "CompanionAbilityComponent.h"

void UAnimNotify_OnSeatedLoopStart::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	ACompanionCharacter* Companion = Cast<ACompanionCharacter>(MeshComp->GetOwner());
	if (Companion && Companion->GetAbilityComponent())
	{
		// ?? Tell component that she is fully seated and ready to receive Essence
		Companion->GetAbilityComponent()->SetIsFullySeated(true);
	}
}