// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "SealedDoor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "BaseCharacter.h"
#include "Utils/InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ASealedDoor::ASealedDoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	SetRootComponent(DoorMesh);
	DoorCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("DoorCollision"));
	DoorCollision->SetupAttachment(DoorMesh);

	DoorInteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("DoorWidgetComponent"));
	DoorInteractWidgetComponent->SetupAttachment(DoorMesh);

	NoEnergyCellWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NoEnergyCellWidgetComponent"));
	NoEnergyCellWidgetComponent->SetupAttachment(DoorMesh);

}

// Called when the game starts or when spawned
void ASealedDoor::BeginPlay()
{
	Super::BeginPlay();

	if (DoorCollision)
	{
		DoorCollision->OnComponentBeginOverlap.AddDynamic(this, &ASealedDoor::OnOverlapBegin);
		DoorCollision->OnComponentEndOverlap.AddDynamic(this, &ASealedDoor::OnOverlapEnd);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("DoorCollision is not valid!"));
	}

	if(!DoorInteractWidgetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("DoorWidgetComponent is not valid!"));
	}
	else
	{
		DoorInteractWidgetComponent->SetVisibility(false);
	}

	if(!NoEnergyCellWidgetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoEnergyCellWidgetComponent is not valid!"));
	}
	else
	{
		NoEnergyCellWidgetComponent->SetVisibility(false);
	}
}

void ASealedDoor::OpenDoor()
{
	// Lerp the door's rotation to open it
	FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
	SetActorLocationAndRotation(GetActorLocation(), FRotator(CurrentRotation.Pitch, CurrentRotation.Yaw + 90.0f, CurrentRotation.Roll));
}

// Called every frame
void ASealedDoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASealedDoor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor == this)
	{
		UE_LOG(LogTemp, Warning, TEXT("OtherActor is not valid or is the same as this actor."));
		return;
	}

	ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor);
	if(!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("OtherActor is not a valid ABaseCharacter."));
		return;
	}

	if (DoorInteractWidgetComponent)
	{
		DoorInteractWidgetComponent->SetVisibility(true);
	}
}

void ASealedDoor::OnInteract()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found!"));
		return;
	}

	if (DoorInteractWidgetComponent)
	{
		DoorInteractWidgetComponent->SetVisibility(false);
	}

	if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
	{
		int32 ItemIndex = Inventory->FindItemByName("Energy Cell");
		if (ItemIndex != INDEX_NONE)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Energy Cell found in inventory. Opening door."));
			DoorCollision->SetGenerateOverlapEvents(false);
			Inventory->RemoveItemByName("Energy Cell");
			OpenDoor();
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("No Energy Cell found in inventory. Cannot open door."));
			if (NoEnergyCellWidgetComponent)
			{
				NoEnergyCellWidgetComponent->SetVisibility(true);
				FTimerHandle DelayHandle = FTimerHandle();
				GetWorld()->GetTimerManager().SetTimer(DelayHandle, [this]()
					{
						NoEnergyCellWidgetComponent->SetVisibility(false);
					}, 3.0f, false);
			}
			
		}
	}
}

void ASealedDoor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(!OtherActor || OtherActor == this)
	{
		return;
	}

	ABaseCharacter* PlayerCharacter = Cast<ABaseCharacter>(OtherActor);
	if(!PlayerCharacter)
	{
		return;
	}

	//DoorWidget->SetVisibility(ESlateVisibility::Hidden);
	if(!DoorInteractWidgetComponent)
	{
		return;
	}

	DoorInteractWidgetComponent->SetVisibility(false);
}

