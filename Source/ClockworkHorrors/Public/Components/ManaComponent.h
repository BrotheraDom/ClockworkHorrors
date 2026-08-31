// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ManaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnManaChanged, float, CurrentMana, float, MaxMana);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLOCKWORKHORRORS_API UManaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UManaComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	float MaxMana = 100.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Mana")
	float CurrentMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mana")
	float ManaRegenRate = 5.0f;

	UPROPERTY(BlueprintAssignable, Category = "Mana")
	FOnManaChanged OnManaChanged;

	UFUNCTION(BlueprintCallable, Category = "Mana")
	bool ConsumeMana(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Mana")
	void RegenerateMana(float Amount);

	UFUNCTION(BlueprintPure, Category = "Mana")
	bool HasEnoughMana(float Amount) const;

	UFUNCTION(BlueprintPure, Category = "Mana")
	float GetManaPercentage() const;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
