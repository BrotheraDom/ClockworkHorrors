// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#include "Utils/InventoryComponent.h"

#include "BaseCharacter.h"
#include "BasePickup.h"
#include "BaseWeapon.h"
#include "WeaponPickup.h"
#include "Utils/HealthComponent.h"
#include "Utils/InventoryItemDataAsset.h"

UInventoryComponent::UInventoryComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    PrimaryComponentTick.bStartWithTickEnabled = false;

    MaxSlots = 10;
    CurrentAvailableSlots = MaxSlots;
}

void UInventoryComponent::BeginPlay()
{
    Super::BeginPlay();

    InventoryItems.SetNum(MaxSlots);
    CurrentAvailableSlots = MaxSlots;

    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            OnInventorySizeIncreased.Broadcast(MaxSlots);
        });
}

void UInventoryComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UInventoryComponent::AddItem(UInventoryItemDataAsset* Item)
{
    if (!IsValid(Item))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot add a null inventory item."));
        return false;
    }

    if (CurrentAvailableSlots <= 0)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Inventory is full. Cannot add %s."),
            *Item->ItemName.ToString()
        );
        return false;
    }

    const int32 ExistingItemIndex = FindItemByName(Item->ItemName);

    if (ExistingItemIndex != INDEX_NONE && !Item->bIsEquippable)
    {
        InventoryItems[ExistingItemIndex].Quantity += Item->Quantity;

        OnItemDataAdded.Broadcast(
            InventoryItems[ExistingItemIndex],
            ExistingItemIndex,
            false
        );

        return true;
    }

    const int32 AvailableSlotIndex = GetFirstAvailableSlot();

    if (AvailableSlotIndex == INDEX_NONE)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("No inventory slot available for %s."),
            *Item->ItemName.ToString()
        );
        return false;
    }

    InventoryItems[AvailableSlotIndex] = FInventorySlotEntry();
    InventoryItems[AvailableSlotIndex].ItemData = Item;
    InventoryItems[AvailableSlotIndex].Quantity = Item->Quantity;
    InventoryItems[AvailableSlotIndex].CurrentBind = AvailableSlotIndex;
    --CurrentAvailableSlots;

    OnItemDataAdded.Broadcast(
        InventoryItems[AvailableSlotIndex],
        AvailableSlotIndex,
        false
    );

    return true;
}

void UInventoryComponent::RemoveItemsByAmount(FName ItemName, int32 Quantity)
{
    if (Quantity <= 0)
    {
        return;
    }

    int32 RemainingQuantity = Quantity;

    while (RemainingQuantity > 0)
    {
        const int32 ItemIndex = FindItemByName(ItemName);

        if (ItemIndex == INDEX_NONE)
        {
            return;
        }

        FInventorySlotEntry& Entry = InventoryItems[ItemIndex];

        const int32 QuantityToRemove =
            FMath::Min(Entry.Quantity, RemainingQuantity);

        Entry.Quantity -= QuantityToRemove;
        RemainingQuantity -= QuantityToRemove;

        OnItemRemoved.Broadcast(ItemName, QuantityToRemove, ItemIndex);

        if (Entry.Quantity <= 0)
        {
            Entry = FInventorySlotEntry();
            ++CurrentAvailableSlots;
        }
    }
}

void UInventoryComponent::RemoveItemByName(FName ItemName)
{
    const int32 ItemIndex = FindItemByName(ItemName);

    if (ItemIndex != INDEX_NONE)
    {
        RemoveItemByIndex(ItemIndex);
    }
}

void UInventoryComponent::RemoveItemByIndex(int32 Index)
{
    if (!InventoryItems.IsValidIndex(Index) ||
        !InventoryItems[Index].IsValidEntry())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid inventory removal index: %d"), Index);
        return;
    }

    OnItemRemoved.Broadcast(
        InventoryItems[Index].GetItemDataName(),
        InventoryItems[Index].Quantity,
        Index
    );

    InventoryItems[Index] = FInventorySlotEntry();
    ++CurrentAvailableSlots;
}

bool UInventoryComponent::HasItem(FName ItemName) const
{
    return FindItemByName(ItemName) != INDEX_NONE;
}

FInventorySlotEntry UInventoryComponent::GetItem(FName ItemName) const
{
    const int32 ItemIndex = FindItemByName(ItemName);

    if (ItemIndex != INDEX_NONE)
    {
        return InventoryItems[ItemIndex];
    }

    return FInventorySlotEntry();
}

void UInventoryComponent::ShowInventory() const
{
    for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
    {
        const FInventorySlotEntry& Entry = InventoryItems[Index];

        if (Entry.IsValidEntry())
        {
            UE_LOG(
                LogTemp,
                Log,
                TEXT("Slot %d: %s, Quantity: %d"),
                Index,
                *Entry.GetItemDataName().ToString(),
                Entry.Quantity
            );
        }
        else
        {
            UE_LOG(LogTemp, Log, TEXT("Slot %d: Empty"), Index);
        }
    }
}

void UInventoryComponent::ClearInventory()
{
    InventoryItems.SetNum(MaxSlots);
    CurrentAvailableSlots = MaxSlots;
}

int32 UInventoryComponent::GetFirstAvailableSlot() const
{
    for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
    {
        if (!InventoryItems[Index].IsValidEntry())
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

int32 UInventoryComponent::FindItemByName(FName ItemName) const
{
    for (int32 Index = 0; Index < InventoryItems.Num(); ++Index)
    {
        if (InventoryItems[Index].IsValidEntry() &&
            InventoryItems[Index].GetItemDataName() == ItemName)
        {
            return Index;
        }
    }

    return INDEX_NONE;
}

void UInventoryComponent::HandleSwapEvent(int32 OriginalIndex, int32 NewIndex)
{
    if (!InventoryItems.IsValidIndex(OriginalIndex) ||
        !InventoryItems.IsValidIndex(NewIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid inventory swap indices."));
        return;
    }

    Swap(InventoryItems[OriginalIndex], InventoryItems[NewIndex]);
    InventoryItems[OriginalIndex].CurrentBind = OriginalIndex;
    OnItemDataAdded.Broadcast(
        InventoryItems[OriginalIndex],
        OriginalIndex,
        true
    );
    InventoryItems[NewIndex].CurrentBind = NewIndex;
    OnItemDataAdded.Broadcast(
        InventoryItems[NewIndex],
        NewIndex,
        true
    );
}

void UInventoryComponent::HandleItemAction(int32 ActionIndex, int32 SlotIndex)
{
    if (!InventoryItems.IsValidIndex(SlotIndex) ||
        !InventoryItems[SlotIndex].IsValidEntry())
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid Slot Index for Item Action."));
        return;
    }

    ABaseCharacter* Player = Cast<ABaseCharacter>(GetOwner());

    if (!IsValid(Player))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("InventoryComponent owner is not an ABaseCharacter.")
        );
        return;
    }

    UInventoryItemDataAsset* ItemData = InventoryItems[SlotIndex].ItemData;

    if (!IsValid(ItemData))
    {
        UE_LOG(
            LogTemp,
            Error,
            TEXT("Inventory entry at %d has no valid ItemData."),
            SlotIndex
        );
        return;
    }

    switch (ActionIndex)
    {
    case 0: // Equip
    {
        ABaseWeapon* Weapon = ItemData->WeaponRef;

        if (!IsValid(Weapon))
        {
            UE_LOG(
                LogTemp,
                Warning,
                TEXT("Item %s has no valid weapon reference."),
                *ItemData->ItemName.ToString()
            );
            return;
        }

        UWeaponPickup* WeaponPickup =
            Weapon->FindComponentByClass<UWeaponPickup>();

        if (!IsValid(WeaponPickup))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Weapon %s has no UWeaponPickup component."),
                *GetNameSafe(Weapon)
            );
            return;
        }

        const int32 WeaponSlotNumber = WeaponPickup->GetInventorySlot();

        UE_LOG(
            LogTemp,
            Warning,
            TEXT(
                "Attempting to equip %s from inventory index %d using weapon slot %d."
            ),
            *GetNameSafe(Weapon),
            SlotIndex,
            WeaponSlotNumber
        );

        Player->EquipWeaponSlot(WeaponSlotNumber);

        for (FInventorySlotEntry& Entry : InventoryItems)
        {
            if (Entry.IsValidEntry() &&
                IsValid(Entry.ItemData) &&
                IsValid(Entry.ItemData->WeaponRef))
            {
                Entry.bIsEquipped = false;
                OnItemDataAdded.Broadcast(Entry, Entry.CurrentBind, true);
            }
        }

        InventoryItems[SlotIndex].bIsEquipped = true;

        OnItemDataAdded.Broadcast(
            InventoryItems[SlotIndex],
            SlotIndex,
            true
        );

        break;
    }

    case 1: // Unequip
    {
        if (!IsValid(ItemData->WeaponRef))
        {
            return;
        }

        UWeaponPickup* basePickup = ItemData->WeaponRef->GetComponentByClass<UWeaponPickup>();
        basePickup->UnequipStoredWeapon();

        InventoryItems[SlotIndex].bIsEquipped = false;

        OnItemDataAdded.Broadcast(
            InventoryItems[SlotIndex],
            SlotIndex,
            true
        );

        break;
    }

    case 2: // Use
    {
        if (!ItemData->bIsHealthItem)
        {
            return;
        }

        UHealthComponent* HealthComponent =
            Player->FindComponentByClass<UHealthComponent>();

        if (!IsValid(HealthComponent))
        {
            UE_LOG(LogTemp, Error, TEXT("Player has no HealthComponent."));
            return;
        }

        if (HealthComponent->CanHeal())
        {
            HealthComponent->Heal(ItemData->HealthRestoreAmount);

            RemoveItemsByAmount(ItemData->ItemName, 1);
        }

        break;
    }

    case 3: // Drop
    {
        if (IsValid(ItemData->WeaponRef))
        {
            Player->DropEquippedWeapon(InventoryItems[SlotIndex]);


            break;
        }

        if (!ItemData->BlueprintClass)
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Item %s has no BlueprintClass to spawn."),
                *ItemData->ItemName.ToString()
            );
            return;
        }

        FActorSpawnParameters SpawnParameters;
        SpawnParameters.Owner = Player;
        SpawnParameters.Instigator = Player;

        const FVector SpawnLocation =
            Player->GetActorLocation() +
            Player->GetActorForwardVector() * 50.0f;

        const FRotator SpawnRotation = Player->GetActorRotation();

        ABasePickup* SpawnedItem =
            GetWorld()->SpawnActor<ABasePickup>(
                ItemData->BlueprintClass,
                SpawnLocation,
                SpawnRotation,
                SpawnParameters
            );

        if (!IsValid(SpawnedItem))
        {
            UE_LOG(
                LogTemp,
                Error,
                TEXT("Could not spawn item from inventory index %d."),
                SlotIndex
            );
            return;
        }

        SpawnedItem->SetItemDataAsset(ItemData);

        RemoveItemByIndex(SlotIndex);
        break;
    }

    default:
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Unknown inventory action index: %d"),
            ActionIndex
        );
        break;
    }
}

void UInventoryComponent::OnInteract()
{
}