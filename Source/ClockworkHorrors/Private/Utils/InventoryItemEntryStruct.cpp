// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/InventoryItemEntryStruct.h"

//InventoryItemEntryStruct::InventoryItemEntryStruct()
//{
//}
//
//InventoryItemEntryStruct::~InventoryItemEntryStruct()
//{
//}

FInventorySlotEntry::FInventorySlotEntry()
{
	ItemData = nullptr;
	Quantity = 0;
	bIsEquipped = false;
	CurrentBind = -1;
	bIsMaxStacked = false;
}
