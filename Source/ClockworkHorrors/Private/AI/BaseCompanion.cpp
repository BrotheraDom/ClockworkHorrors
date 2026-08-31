// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "AI/BaseCompanion.h"
#include "Utils/HealthComponent.h"
#include "Components/ManaComponent.h"
#include "AIController.h"
#include <BrainComponent.h>
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/EnemyInterface.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ABaseCompanion::ABaseCompanion()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	ManaComponent = CreateDefaultSubobject<UManaComponent>(TEXT("ManaComponent"));
	SpellManager = CreateDefaultSubobject<USpellManager>(TEXT("SpellManager"));

	if (UCharacterMovementComponent* MoveComp =
		GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}

}

// Called when the game starts or when spawned
void ABaseCompanion::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnCharacterDeath.AddDynamic(
			this,
			&ABaseCompanion::HandleDeath
		);
	}
	
}

// Called every frame
void ABaseCompanion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCompanion::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseCompanion::HandleActionFinished()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBrainComponent* BrainComp = AIController->GetBrainComponent();
		if (BrainComp)
		{
			FAIMessage Message(ActionFinishedMessage, this);
			FAIMessage::Send(AIController, Message);
			UE_LOG(LogTemp, Warning, TEXT("Sent ActionFinished AI message"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BrainComponent is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is null"));
	}
}

bool ABaseCompanion::CastSpellSlot(int spellslot, AActor* Target)
{
	return false;
}

void ABaseCompanion::UpdateTarget(AActor* NewTarget, float damage)
{
	IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(NewTarget);
	if (!EnemyInterface)
	{
		UE_LOG(LogTemp, Error, TEXT("NewTarget does not implement IEnemyInterface"));
		return;
	}
	
		
	
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			if (BlackboardComp->GetValueAsObject("Target"))
			{
				float randomChance = FMath::FRand();
				if (randomChance <= damage)
				{
					BlackboardComp->SetValueAsObject("Target", NewTarget);
				}
				UE_LOG(LogTemp, Warning, TEXT("Updating target in blackboard to: %s"), *NewTarget->GetName());
			}
			else
			{
				BlackboardComp->SetValueAsObject("Target", NewTarget);
				UE_LOG(LogTemp, Warning, TEXT("Updated target in blackboard"));
			}
			
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is null"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AIController is null"));
	}
}

void ABaseCompanion::HandleDeath()
{
	Destroy();
}
