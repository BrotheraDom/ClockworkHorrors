// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/ExperienceComponent.h"

// Sets default values for this component's properties
UExperienceComponent::UExperienceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	ExperiencePoints = 0.0f;
	CurrentLevel = 0;
	MaxExperiencePoints = 100.0f;
	NextLevelExperienceMultiplier = 1.5f;
	SkillPoints = 0;

	// ...
}


// Called when the game starts
void UExperienceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UExperienceComponent::LevelUp()
{
	CurrentLevel++;
	ExperiencePoints = 0.0f;
	MaxExperiencePoints *= NextLevelExperienceMultiplier;
	SkillPoints++;

	UE_LOG(LogTemp, Error, TEXT("Level Up! New Level: %d"), CurrentLevel);
}


// Called every frame
void UExperienceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UExperienceComponent::AddExperience(float Amount)
{
	ExperiencePoints += Amount;
	if (ExperiencePoints >= MaxExperiencePoints)
	{
		float ExcessExperience = ExperiencePoints - MaxExperiencePoints;
		LevelUp();
		ExperiencePoints += ExcessExperience;
	}

	UE_LOG(LogTemp, Error, TEXT("Added Experience: %f, Current Experience: %f"), Amount, ExperiencePoints);
}

