#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CompanionCharacter.generated.h"

class UCompanionAbilityComponent;

UCLASS()
class CLOCKWORKHORRORS_API ACompanionCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ACompanionCharacter();
    UFUNCTION(BlueprintPure, Category = "Companion")
    UCompanionAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    TObjectPtr<UCompanionAbilityComponent> AbilityComponent;
};