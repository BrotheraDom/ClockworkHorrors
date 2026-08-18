// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class CLOCKWORKHORRORS_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, Category = "InventoryDragDropOperation")
	class UInventorySlotWidget* SourceInventorySlot;
	
};
