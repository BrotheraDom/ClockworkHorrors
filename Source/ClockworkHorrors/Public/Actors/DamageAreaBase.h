// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageAreaBase.generated.h"

class UBoxComponent;
class ABaseCharacter;

UCLASS()
class CLOCKWORKHORRORS_API ADamageAreaBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADamageAreaBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Damage Area")
	TObjectPtr<UBoxComponent> DamageBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
	bool bDamagePlayer = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
	bool bDamageOnEntry = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area")
	bool bDamageOverTime = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area", meta = (ClampMin = "0.0"))
	float DamageAmount = 1.0f;

	// Seconds between repeated damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage Area", meta = (ClampMin = "0.1"))
	float DamageInterval = 1.0f;

	UFUNCTION()
	void OnDamageBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDamageBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void DamagePlayer();

	void ApplyDamageToCharacter(ABaseCharacter* Character);

private:
	UPROPERTY()
	TObjectPtr<ABaseCharacter> PlayerInDamageArea = nullptr;

	FTimerHandle DamageTimerHandle;
};
