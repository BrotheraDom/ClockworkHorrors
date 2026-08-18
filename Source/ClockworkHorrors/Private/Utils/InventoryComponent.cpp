// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Utils/InventoryComponent.h"
#include "Utils/InventoryItemDataAsset.h"
#include "BasePickup.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// ...

	MaxSlots = 10; // Default maximum number of slots in the inventory
	CurrentAvailableSlots = MaxSlots; // Initialize current available slots to maximum
	InventoryItems = TArray<FInventorySlotEntry>(); // Initialize the inventory items array
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryItems.SetNum(MaxSlots); // Initialize the inventory items array with the maximum number of slots
	CurrentAvailableSlots = MaxSlots; // Reset current available slots to maximum

	//UE_LOG(LogTemp, Warning, TEXT("Inventory Component Initialized with Max slots: %d and Current Available Slots: %d. TRUE InventoryItems reserved: %d"), MaxSlots, CurrentAvailableSlots, InventoryItems.Max());

	//ShowInventory();

	// Wait for the game to start before initializing the inventory
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		//UE_LOG(LogTemp, Warning, TEXT("Inventory Component Initialized with Max slots: %d and Current Available Slots: %d. TRUE InventoryItems reserved: %d"), MaxSlots, CurrentAvailableSlots, InventoryItems.Max());
		OnInventorySizeIncreased.Broadcast(MaxSlots);
		});

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UInventoryComponent::AddItem(UInventoryItemDataAsset* Item)
{
	if (CurrentAvailableSlots <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Inventory is full! Cannot add item: %s"), *Item->ItemName.ToString());
		return false; // No available slots to add items
	}

	//UE_LOG(Game, Log, TEXT("UInventoryComponent::AddItemToInventory CURRENT INVENTORY SLOTS AVAILABLE: %d"), CurrentInventorySlotAvailable);

	//Does the item already exist in the inventory?
	int32 Index = FindItemByName(Item->ItemName);
	// is the index valid? (Item Already Exist) AND is the item not equippable?
	if (Index != INDEX_NONE && !Item->bIsEquippable)
	{
		// If the item already exists in the inventory, just increase the quantity
		InventoryItems[Index].Quantity += Item->Quantity;
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent: Updated Item Quantity: %s, New Quantity: %d, Slot Index: %d"), *InventoryItems[Index].ItemData->ItemName.ToString(), InventoryItems[Index].Quantity, Index);
		OnItemDataAdded.Broadcast(InventoryItems[Index], Index, false); // This is to notify the UI that an item quantity has been updated
		return true;
	}

	// The item does not exist in the inventory or is equippable, so we need to add it as a new item
	int32 index = GetFirstAvailableSlot();
	if (index != -1)
	{
		InventoryItems[index] = FInventorySlotEntry();
		InventoryItems[index].ItemData = Item;
		InventoryItems[index].Quantity = Item->Quantity; // Set the quantity to the item's quantity

		CurrentAvailableSlots--;
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent: Added NEW Item: %s, Quantity: %d, Slot Index: %d"), *InventoryItems[index].ItemData->ItemName.ToString(), InventoryItems[index].Quantity, index);
		OnItemDataAdded.Broadcast(InventoryItems[index], index, false); // This is to notify the UI that a new item has been added
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent: No available slot to add: %s"), *Item->ItemName.ToString());
		return false; // No available slot to add the item
	}

	//ShowInventory(); // Comment this out if you don't want to log the inventory after every addition

	return true;
}

void UInventoryComponent::RemoveItemsByAmount(FName ItemName, int32 Quantity)
{
	int32 TotalQuantity = Quantity;
	do // Loops through the inventory to remove the requested quantity, even if it spans multiple slots
	{
		int32 Index = FindItemByName(ItemName);

		// If the item exists in the inventory
		if (Index != INDEX_NONE)
		{
			// check if we have enough quantity to remove in a single slot
			if (InventoryItems[Index].Quantity >= TotalQuantity)
			{
				InventoryItems[Index].Quantity -= TotalQuantity;
				OnItemRemoved.Broadcast(ItemName, TotalQuantity, Index); // This is to notfy the UI that an item has been removed
				TotalQuantity = 0; // Set to zero since we have removed the requested quantity
				//UE_LOG(Game, Warning, TEXT("UInventoryComponent: Removed Item: %s, Quantity: %d, Slot Index: %d"), *ItemName.ToString(), Quantity, Index);
			}
			// Not enough quantity to remove from a single slot, set this slot to zero
			else
			{
				//UE_LOG(Game, Warning, TEXT("Not enough quantity to remove! Setting to zero."));
				TotalQuantity -= InventoryItems[Index].Quantity; // Decrease the total quantity by the amount removed
				OnItemRemoved.Broadcast(ItemName, InventoryItems[Index].Quantity, Index); // This is to notfy the UI that an item has been removed
				InventoryItems[Index].Quantity = 0; // Set to zero if trying to remove more than available

			}


			// Checks to see if the item quantity is zero or less after removal
			if (InventoryItems[Index].Quantity <= 0)
			{
				// Remove item if quantity is zero or less
				InventoryItems[Index] = FInventorySlotEntry(); // Reset the item to default state
				CurrentAvailableSlots++;
			}
		}
		else
		{
			//UE_LOG(Game, Warning, TEXT("Item not found in inventory!"));
			return; // Exit if item not found
		}

		//UE_LOG(Game, Error, TEXT("UInventoryComponent: Total Quantity Remaining: %d"), TotalQuantity);
	} while (TotalQuantity > 0); // Continue removing until all requested quantity is removed

	//ShowInventory();
}

void UInventoryComponent::RemoveItemByName(FName ItemName)
{
	int32 Index = FindItemByName(ItemName);

	// If the item exists in the inventory
	if (Index != INDEX_NONE)
	{
		InventoryItems[Index] = FInventorySlotEntry(); // Reset the item to default state
		CurrentAvailableSlots++;
		//ShowInventory();
	}
	else
	{
		//UE_LOG(Game, Warning, TEXT("Item not found in inventory!"));
		return; // Exit if item not found
	}
}

void UInventoryComponent::RemoveItemByIndex(int32 Index)
{
	if (Index >= 0 && Index < InventoryItems.Num())
	{
		OnItemRemoved.Broadcast(InventoryItems[Index].GetItemDataName(), InventoryItems[Index].Quantity, Index); // This is to notfy the UI that an item has been removed
		InventoryItems[Index] = FInventorySlotEntry(); // Reset the item to default state
		CurrentAvailableSlots++;
		//ShowInventory();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid index for removing item from inventory!"));
	}
}

bool UInventoryComponent::HasItem(FName ItemName) const
{
	return false;
}

FInventorySlotEntry UInventoryComponent::GetItem(FName ItemName) const
{
	int32 ItemIndex = FindItemByName(ItemName);
	if(ItemIndex != INDEX_NONE)
	{
		return InventoryItems[ItemIndex];
	}
	return FInventorySlotEntry(); // Return an invalid entry if the item is not found
}

void UInventoryComponent::ShowInventory() const
{
	for(int i = 0; i < InventoryItems.Num(); i++)
	{
		if(InventoryItems[i].IsValidEntry())
		{
			UE_LOG(LogTemp, Log, TEXT("Slot %d: Item Name: %s, Quantity: %d"), i, *InventoryItems[i].GetItemDataName().ToString(), InventoryItems[i].Quantity);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Slot %d: Empty"), i);
		}
	}
}

void UInventoryComponent::ClearInventory()
{
}

int32 UInventoryComponent::GetFirstAvailableSlot() const
{
	for (int32 i = 0; i < InventoryItems.Num(); i++)
	{
		if (!InventoryItems[i].IsValidEntry())
		{
			return i; // Return the index of the first available slot
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("Slot %d is occupied by item: %s"), i+1, *InventoryItems[i].GetItemDataName().ToString());
		}
	}

	return INDEX_NONE; // No available slot found
}

int32 UInventoryComponent::FindItemByName(FName ItemName) const
{
	for(int i = 0; i < InventoryItems.Num(); i++)
	{
		if(InventoryItems[i].GetItemDataName() == ItemName)
		{
			return i; // Return the index of the item if found
		}
	}

	return INDEX_NONE;
}

void UInventoryComponent::HandleSwapEvent(int32 OriginalIndex, int32 NewIndex)
{
	if (OriginalIndex < 0 || OriginalIndex >= InventoryItems.Num() || NewIndex < 0 || NewIndex >= InventoryItems.Num())
	{
		//UE_LOG(Game, Warning, TEXT("Invalid indices for swapping items! OldIndex: %d, TargetIndex: %d"), OriginalIndex, NewIndex);
		return;
	}

	FInventorySlotEntry TempItem = InventoryItems[OriginalIndex];
	InventoryItems[OriginalIndex] = InventoryItems[NewIndex];
	InventoryItems[NewIndex] = TempItem;
	//Broadcasting the new item added event
	OnItemDataAdded.Broadcast(InventoryItems[NewIndex], NewIndex, true);

	//Broadcasting the old item added event
	OnItemDataAdded.Broadcast(InventoryItems[OriginalIndex], OriginalIndex, true);

	ShowInventory(); // Comment this out if you don't want to log the inventory after every swap
}

void UInventoryComponent::HandleItemAction(int32 ActionIndex, int32 SlotIndex)
{
	if(SlotIndex == -1)
	{UE_LOG(LogTemp, Warning, TEXT("Invalid Slot Index for Item Action!"));
	return;
	}


	switch (ActionIndex)
	{
	case 0: // Equip
	{
		// Handle equip logic here
		break;
	}
	case 1: // Unequip
	{
		// Handle unequip logic here
		break;
	}
	case 2: // Use
	{
		// Handle use/health logic here
		break;
	}
	case 3: // Drop
	{
		FActorSpawnParameters spawnParams;
		spawnParams.Instigator = nullptr; // No instigator for the spawner itself
		spawnParams.Owner = GetOwner(); // Set the owner to the spawner itself

		FVector SpawnLocation = GetOwner()->GetActorLocation() + FVector(0.f, 50.f, 0.f); // Spawn in front of the player
		FRotator SpawnRotation = GetOwner()->GetActorRotation().Vector().Rotation(); // Spawn facing the player

		ABasePickup* SpawnedItem = GetWorld()->SpawnActor<ABasePickup>(InventoryItems[SlotIndex].ItemData->BlueprintClass, SpawnLocation, SpawnRotation, spawnParams);

		if (!SpawnedItem)
		{
			UE_LOG(LogTemp, Error, TEXT("ItemClass is not a valid ABasePickup!"));
			return;
		}

		SpawnedItem->SetItemDataAsset(InventoryItems[SlotIndex].ItemData);

		RemoveItemByIndex(SlotIndex);
		break;
	}
	default:
		break;
	}
}

void UInventoryComponent::OnInteract()
{
}

