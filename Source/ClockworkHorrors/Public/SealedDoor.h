// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "SealedDoor.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API ASealedDoor : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASealedDoor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OpenDoor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UBoxComponent* DoorCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	class UWidgetComponent* DoorInteractWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UWidgetComponent* NoEnergyCellWidgetComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnInteract() override;

	UFUNCTION(BlueprintCallable, Category = "Door")
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
