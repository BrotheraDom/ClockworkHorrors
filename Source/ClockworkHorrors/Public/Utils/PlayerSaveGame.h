// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UPlayerSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	float currentHealth;
	UPROPERTY()
	float Experience;
	UPROPERTY()
	int32 Level;
	UPROPERTY()
	int32 SkillPoints;
	UPROPERTY()
	TArray<struct FInventorySlotEntry> Inventory;
	UPROPERTY()
	int32 currentEquippedSlot;
	TArray<int> ammo;
	bool SavePlayerData(float health, float exp, int32 curLevel, int32 CurrentSkillPoints, TArray<struct FInventorySlotEntry> curInventory, int32 equippedSlot);
	static UPlayerSaveGame* LoadPlayerData();
};
