// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/InventoryItemDataAsset.h"

UInventoryItemDataAsset::UInventoryItemDataAsset() : ItemName(NAME_None), /*SlotSpaces(0),*/ Icon(nullptr), bIsEquippable(false), bIsThrowable(false), bIsHealthItem(false) {
	//BlueprintClass = nullptr;
	ThrowClass = nullptr;
	bIsAttachment = false;
	HealthRestoreAmount = 0.0f;
	bIsCombinable = false;
	bIsCraftingMaterial = false;
	/*DurationAmount = 0.0f;
	AttachmentData = FAttachmentData();
	AttachmentData.Type = EAttachmentType::None;
	CurrentBind = -1;*/
}
