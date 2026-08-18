// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "PlayerAnimation.h"
//#include "KismetAnimationLibrary.h"

void UPlayerAnimation::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();

	if (Pawn)
	{
		Velocity = Pawn->GetVelocity().Size();
		//Direction = UKismetAnimationLibrary::CalculateDirection(Pawn->GetVelocity(), Pawn->GetActorRotation());
	}
}

void UPlayerAnimation::SwingingAnimation()
{
	Montage_Play(swingingAsset);
}
