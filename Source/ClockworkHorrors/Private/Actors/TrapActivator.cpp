// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "Actors/TrapActivator.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseCharacter.h"
#include "Utils/InventoryComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
ATrapActivator::ATrapActivator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	ActivatorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ActivatorMesh"));
	SetRootComponent(ActivatorMesh);

	ActivationZone = CreateDefaultSubobject<UBoxComponent>(TEXT("ActivationZone"));
	ActivationZone->SetupAttachment(RootComponent);

	TrapsToActivate = TArray<AActor*>();

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(RootComponent);

	NoEnergyCellWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NoEnergyCellWidgetComponent"));
	NoEnergyCellWidgetComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATrapActivator::BeginPlay()
{
	Super::BeginPlay();

	ActivationZone->OnComponentBeginOverlap.AddDynamic(this, &ATrapActivator::OverlapBegin);
	ActivationZone->OnComponentEndOverlap.AddDynamic(this, &ATrapActivator::OverlapEnd);

	/*ActivationWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), ActivationWidgetClass);
	ActivationWidget->AddToViewport(-1);
	ActivationWidget->SetVisibility(ESlateVisibility::Hidden);
	NoEnergyCellWidget = CreateWidget<UUserWidget>(GetWorld()->GetFirstPlayerController(), NoEnergyCellWidgetClass);
	NoEnergyCellWidget->AddToViewport(-1);
	NoEnergyCellWidget->SetVisibility(ESlateVisibility::Hidden);*/

	if(!InteractWidgetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractWidgetComponent is not valid!"));
	}
	else
	{
		InteractWidgetComponent->SetVisibility(false);
	}

	if(!NoEnergyCellWidgetComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("NoEnergyCellWidgetComponent is not valid!"));
	}
	else
	{
		NoEnergyCellWidgetComponent->SetVisibility(false);
	}
	bAlreadyHasEnergyCell = false;
	
}

void ATrapActivator::OverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(!OtherActor || OtherActor == this)
	{
		return;
	}

	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);
	if(!Player)
	{
		return;
	}

	if(!InteractWidgetComponent)
	{
		return;
	}

	InteractWidgetComponent->SetVisibility(true);
}

void ATrapActivator::OverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	ABaseCharacter* Player = Cast<ABaseCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	if(!InteractWidgetComponent)
	{
		return;
	}

	InteractWidgetComponent->SetVisibility(false);
}

// Called every frame
void ATrapActivator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrapActivator::OnInteract()
{
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found!"));
		return;
	}

	if(InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
	}

	if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
	{
		int32 ItemIndex = Inventory->FindItemByName("Energy Cell");
		if (ItemIndex != INDEX_NONE)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Energy Cell found in inventory."));
			ActivationZone->SetGenerateOverlapEvents(false);
			Inventory->RemoveItemByName("Energy Cell");
			bAlreadyHasEnergyCell = true;
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("No Energy Cell found in inventory."));
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

