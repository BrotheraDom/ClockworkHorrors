#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CompanionAbilityComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEssenceChanged, int32, CurrentEssence, int32, MaxEssence);

UENUM(BlueprintType)
enum class EOrbFlightState : uint8
{
	Orbiting,
	MovingToHand,
	AttachedToHand,
	FlyingToTarget
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class CLOCKWORKHORRORS_API UCompanionAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCompanionAbilityComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable, Category = "Companion|Essence")
	FOnEssenceChanged OnEssenceChanged;

	// --- PUBLIC ACCESSORS FOR TASKS ---
	UFUNCTION(BlueprintCallable, Category = "Companion|Essence")
	int32 GetCurrentEssence() const { return CurrentEssence; }

	UFUNCTION(BlueprintCallable, Category = "Companion|Essence")
	int32 GetMaxEssence() const { return MaxEssence; }

	UFUNCTION(BlueprintCallable, Category = "Companion|Rest")
	bool GetIsResting() const { return bIsResting; }

	UFUNCTION(BlueprintCallable, Category = "Companion|Rest")
	bool GetIsFullySeated() const { return bIsFullySeated; }

	UFUNCTION(BlueprintCallable, Category = "Companion|Rest")
	void SetIsResting(bool bInResting) { bIsResting = bInResting; }

	UFUNCTION(BlueprintCallable, Category = "Companion|Rest")
	void SetIsFullySeated(bool bInSeated) { bIsFullySeated = bInSeated; }

	// --- ABILITY PROPERTIES ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Healing Touch")
	int32 HealingTouchCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Healing Touch")
	class UAnimMontage* CastHealMontage;

	// --- ESSENCE METHODS ---
	UFUNCTION(BlueprintCallable, Category = "Companion|Essence")
	void AddEssence(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Companion|Essence")
	bool ConsumeEssence(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Abilities")
	bool PerformHealingTouch(AActor* TargetActor);

	UFUNCTION(BlueprintCallable, Category = "Abilities|Healing Touch")
	void Notify_SpawnHandOrb();

	UFUNCTION(BlueprintCallable, Category = "Abilities|Healing Touch")
	void Notify_LaunchHandOrb(AActor* TargetPlayer = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Companion|Abilities")
	EOrbFlightState GetCurrentOrbState() const { return CurrentOrbState; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion|Essence")
	int32 MaxEssence = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion|Essence")
	int32 CurrentEssence = 0;

	UPROPERTY(EditAnywhere, Category = "Companion|Essence|Visuals")
	TSubclassOf<AActor> OrbActorClass;

	UPROPERTY(EditAnywhere, Category = "Companion|Essence|Visuals")
	float OrbitRadius = 85.0f;

	UPROPERTY(EditAnywhere, Category = "Companion|Essence|Visuals")
	float OrbitHeightOffset = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Companion|Essence|Visuals")
	float OrbitSpeed = 90.0f;

	UPROPERTY(EditAnywhere, Category = "Companion|Essence|Visuals")
	FVector OrbScale = FVector(0.08f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Healing Touch")
	float HealingTouchAmount = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities|Healing Touch")
	float FlightSpeed = 12.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Rest")
	bool bIsResting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Companion|Rest")
	bool bIsFullySeated = false;

	float EssenceAccumulator = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Companion|Essence")
	float DistancePerEssence = 500.0f;

	float AccumulatedDistance = 0.0f;
	FVector LastFrameLocation = FVector::ZeroVector;

private:
	float CurrentOrbitAngle = 0.0f;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ActiveOrbActors;

	UPROPERTY()
	AActor* ActiveCastingOrb = nullptr;

	UPROPERTY()
	AActor* CurrentTargetPlayer = nullptr;

	EOrbFlightState CurrentOrbState = EOrbFlightState::Orbiting;

	void SyncOrbActors();
	void UpdateOrbPositions();
	void ProcessCastingOrbFlight(float DeltaTime);
};