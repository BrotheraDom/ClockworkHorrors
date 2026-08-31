#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h" // <-- All standard/UE includes go HERE

#include "UBTService_UpdateCompanionSpeed.generated.h" // <-- MUST BE THE LAST INCLUDE!

UCLASS()
class CLOCKWORKHORRORS_API UBTService_UpdateCompanionSpeed : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_UpdateCompanionSpeed();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	/** Blackboard key for the Target Actor (Player) */
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetActorKey;

	/** Distance threshold below which the companion walks */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float WalkDistanceThreshold = 400.0f;

	/** Distance threshold above which the companion runs at full speed */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float RunDistanceThreshold = 800.0f;

	/** Speed when close to the target */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float WalkSpeed = 200.0f;

	/** Speed when catching up to the target */
	UPROPERTY(EditAnywhere, Category = "Speed")
	float RunSpeed = 600.0f;
};