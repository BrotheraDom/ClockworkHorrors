// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ExperienceComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CLOCKWORKHORRORS_API UExperienceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UExperienceComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Experience")
	void LevelUp();

	/** Multiplier applied to the required EXP each level (e.g. 100 -> 150 -> 225 at 1.5x). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float NextLevelExperienceMultiplier = 1.5f;

	/** Base experience required to reach level 1. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float BaseExperienceRequired = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int32 CurrentLevel = 1;

private:

	/// <summary>
	/// Current Experience Points of the player.
	/// </summary>
	float ExperiencePoints;


	/// <summary>
	/// Max Experience Points before leveling up.
	/// </summary>
	float MaxExperiencePoints;

	/// <summary>
	/// How many skill points the player has available.
	/// </summary>
	int32 SkillPoints = 0;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Experience")
	void AddExperience(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Experience")
	float GetExperiencePoints() const { return ExperiencePoints; }
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void SetExperiencePoints(float exp) { ExperiencePoints = exp; }
	UFUNCTION(BlueprintCallable, Category = "Experience")
	float GetMaxExperiencePoints() const { return MaxExperiencePoints; }

	UFUNCTION(BlueprintCallable, Category = "Experience")
	float GetExperiencePercent() const
	{
		if (MaxExperiencePoints <= 0.0f)
		{
			return 0.0f;
		}

		return FMath::Clamp(ExperiencePoints / MaxExperiencePoints, 0.0f, 1.0f);
	}

	UFUNCTION(BlueprintCallable, Category = "Experience")
	int32 GetLevel() const { return CurrentLevel; }
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void SetLevel(int32 level) { CurrentLevel = level;  }
	UFUNCTION(BlueprintCallable, Category = "Experience")
	int32 GetSkillPoints() const { return SkillPoints; }
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void SetSkillPoints(int32 skill) { SkillPoints = skill; }
};
