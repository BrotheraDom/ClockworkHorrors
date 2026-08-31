#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VisualHealOrb.generated.h"

UENUM(BlueprintType)
enum class EOrbVisualState : uint8
{
	Idle,
	MovingToHand,
	AttachedToHand,
	FlyingToTarget,
	ReachedTarget
};

UCLASS()
class CLOCKWORKHORRORS_API AVisualHealOrb : public AActor
{
	GENERATED_BODY()

public:
	AVisualHealOrb();

	virtual void Tick(float DeltaTime) override;

	/** Begins VInterp to hand socket */
	UFUNCTION(BlueprintCallable, Category = "Visual Heal Orb")
	void MoveToHandSocket(USkeletalMeshComponent* MeshComp, FName SocketName);

	/** Detaches from hand and flies toward target actor, healing on arrival */
	UFUNCTION(BlueprintCallable, Category = "Visual Heal Orb")
	void FlyToTargetActor(AActor* TargetActor, float HealAmount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual Heal Orb")
	class UStaticMeshComponent* OrbMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Heal Orb")
	float GatherSpeed = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual Heal Orb")
	float FlightSpeed = 10.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visual Heal Orb")
	EOrbVisualState CurrentState = EOrbVisualState::Idle;

private:
	UPROPERTY()
	USkeletalMeshComponent* TargetMeshComponent = nullptr;

	FName TargetSocketName;

	UPROPERTY()
	AActor* TargetPlayerActor = nullptr;

	float PendingHealAmount = 0.0f;

	void OnReachPlayer();
};