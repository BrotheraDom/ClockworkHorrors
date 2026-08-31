// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "BasePickup.generated.h"

UENUM(BlueprintType)
enum class EPickupMeshType : uint8
{
	Static,
	Skeletal
};

UCLASS()
class CLOCKWORKHORRORS_API ABasePickup : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	UFUNCTION(BlueprintCallable, Category = "Pickup")
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	float PickupRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	class USphereComponent* PickupRangeSphere;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	class UPrimitiveComponent* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	class UInventoryItemDataAsset* ItemDataAsset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	bool isStatic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pickup")
	EPickupMeshType MeshType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	class UStaticMesh* StaticMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Pickup")
	class USkeletalMesh* SkeletalMesh;

private:
	bool bIsPickedUp = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnInteract() override;
	
	class UInventoryItemDataAsset* GetItemDataAsset();

	void SetItemDataAsset(UInventoryItemDataAsset* NewItemDataAsset);
};
