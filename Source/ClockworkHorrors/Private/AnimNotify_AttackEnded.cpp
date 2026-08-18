// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AnimNotify_AttackEnded.h"
#include "PlayerAnimation.h"
void UAnimNotify_AttackEnded::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	UPlayerAnimation* playerAnim = Cast < UPlayerAnimation>(MeshComp->GetAnimInstance());
	if (playerAnim)
		playerAnim->OnAttackEnded.Broadcast();
}
