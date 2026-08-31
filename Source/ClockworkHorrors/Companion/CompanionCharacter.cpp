#include "CompanionCharacter.h"
#include "CompanionAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CompanionAbilityComponent.h"

ACompanionCharacter::ACompanionCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Ensure Movement Component has speed and orientation set
    bUseControllerRotationYaw = false;
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->bOrientRotationToMovement = true;
        GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f);
        GetCharacterMovement()->MaxWalkSpeed = 400.0f;
    }

    AbilityComponent = CreateDefaultSubobject<UCompanionAbilityComponent>(TEXT("AbilityComponent"));

    AIControllerClass = ACompanionAIController::StaticClass();
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}