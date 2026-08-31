#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RiflePickup.generated.h"

class USkeletalMeshComponent;
class USphereComponent;

UCLASS()
class CLOCKWORKHORRORS_API ARiflePickup : public AActor
{
	GENERATED_BODY()

public:
	ARiflePickup();

	USkeletalMeshComponent* GetRifleMesh() const { return RifleMesh; }

private:
	UPROPERTY(VisibleAnywhere, Category = "Rifle Pickup")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, Category = "Rifle Pickup")
	TObjectPtr<USkeletalMeshComponent> RifleMesh;

	UPROPERTY(VisibleAnywhere, Category = "Rifle Pickup")
	TObjectPtr<USphereComponent> PickupRange;

	UFUNCTION()
	void BoundOverlap(UPrimitiveComponent* OverlappedComponenet, AActor* actor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void LeaveOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	bool playerInRange;
};