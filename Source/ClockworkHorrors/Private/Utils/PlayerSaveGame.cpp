// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/PlayerSaveGame.h"
#include <Kismet/GameplayStatics.h>
#include "BaseCharacter.h"
#include "Utils/InventoryItemEntryStruct.h"
bool UPlayerSaveGame::SavePlayerData(float health, float exp, int32 curLevel, int32 CurrentSkillPoints, TArray<struct FInventorySlotEntry> curInventory, int32 equippedSlot)
{
	UPlayerSaveGame* SaveData = NewObject<UPlayerSaveGame>();
	if (!SaveData)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UPlayerSaveGame object."));
		return false;
	}
	SaveData->currentHealth = health;
	SaveData->Experience = exp;
	SaveData->Level = curLevel;
	SaveData->SkillPoints = CurrentSkillPoints;
	SaveData->Inventory = curInventory;
	SaveData->currentEquippedSlot = equippedSlot;
	if (!UGameplayStatics::SaveGameToSlot(SaveData, TEXT("PlayerSaveSlot"), 0))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to save player to slot."));
		return false;
	}
	UE_LOG(LogTemp, Log, TEXT("Player saved successfully."));
	return true;
}

UPlayerSaveGame* UPlayerSaveGame::LoadPlayerData()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("PlayerSaveSlot"), 0)) {
		UPlayerSaveGame* LoadedData = Cast<UPlayerSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("PlayerSaveSlot"), 0));

		if (LoadedData) {
			return LoadedData;
		}
		else
		{
			return nullptr;
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No existing Player save Data. Creating new save data."));
		UPlayerSaveGame* NewSaveData = NewObject<UPlayerSaveGame>();
		if (NewSaveData) {
			TArray<FInventorySlotEntry> slots;
			NewSaveData->SavePlayerData(5, 0, 1, 0,slots,0);
			return NewSaveData;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Failed to create new UPlayerSaveGame object."));
		}
		return nullptr;
	}
}
