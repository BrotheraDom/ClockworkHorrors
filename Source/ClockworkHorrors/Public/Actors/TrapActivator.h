// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "TrapActivator.generated.h"

UCLASS()
class CLOCKWORKHORRORS_API ATrapActivator : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrapActivator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	class TArray<AActor*> TrapsToActivate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trap")
	class UBoxComponent* ActivationZone;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trap")
	class UStaticMeshComponent* ActivatorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Door")
	class UWidgetComponent* InteractWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UWidgetComponent* NoEnergyCellWidgetComponent;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TSubclassOf<class UUserWidget> ActivationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	TSubclassOf<class UUserWidget> NoEnergyCellWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UUserWidget* ActivationWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	class UUserWidget* NoEnergyCellWidget;*/

private:
	bool bAlreadyHasEnergyCell = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteract() override;

};
