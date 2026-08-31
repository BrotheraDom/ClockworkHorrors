// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "BasePickup.h"
#include "Components/SphereComponent.h"
#include "BaseCharacter.h"
#include "Utils/InventoryComponent.h"

// Sets default values
ABasePickup::ABasePickup() : MeshType(EPickupMeshType::Static)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// You can't check for this here because the constructor is called before the properties are set. So you need to check this in BeginPlay or PostInitializeComponents.
	/*if (isStatic)
	{
		PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	}
	else
	{
		PickupMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PickupMesh"));
	}

	SetRootComponent(PickupMesh);
	*/

	PickupRange = 150.0f;

	PickupRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupRangeSphere"));
	PickupRangeSphere->SetSphereRadius(PickupRange);

	bIsPickedUp = false;

}

void ABasePickup::BeginPlay()
{
	Super::BeginPlay();

	switch (MeshType)
	{
	case EPickupMeshType::Static:
	{		
		if (UStaticMeshComponent* MeshComponent = NewObject<UStaticMeshComponent>(this, TEXT("PickupMesh")))
		{
			MeshComponent->RegisterComponent();
			MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			MeshComponent->SetStaticMesh(StaticMesh);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
			MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
			MeshComponent->SetSimulatePhysics(true);
			MeshComponent->SetEnableGravity(true);

			PickupMesh = MeshComponent;
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MeshComponent is invalid"));
			}
		}

		break;
	}

	case EPickupMeshType::Skeletal:
	{	
		if (USkeletalMeshComponent* MeshComponent = NewObject<USkeletalMeshComponent>(this, TEXT("PickupMesh")))
		{
			MeshComponent->RegisterComponent();
			MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
			MeshComponent->SetSkeletalMesh(SkeletalMesh);
			MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
			MeshComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
			MeshComponent->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
			MeshComponent->SetSimulatePhysics(true);
			MeshComponent->SetEnableGravity(true);

			PickupMesh = MeshComponent;
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("MeshComponent is invalid"));
			}
		}

		break;
	}
	}

	// Old logic for creating the mesh component based on isStatic flag. Replaced with switch statement above using enum.
	/*if (isStatic)
	{
		if (UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("PickupMesh")))
		{
			Mesh->RegisterComponent();
			Mesh->AttachToComponent(PickupRangeSphere, FAttachmentTransformRules::KeepRelativeTransform);
			Mesh->SetStaticMesh(StaticMesh);
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

		//PickupMesh = NewObject<UStaticMeshComponent>(this, TEXT("PickupMesh"));
		if (PickupMesh)
		{
			UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(PickupMesh);
			mesh->SetStaticMesh(StaticMesh);
			SetRootComponent(mesh);
			PickupRangeSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}

	}
	else
	{
		//PickupMesh = NewObject<USkeletalMeshComponent>(this, TEXT("PickupMesh"));
		if (PickupMesh)
		{
			USkeletalMeshComponent* mesh = Cast<USkeletalMeshComponent>(PickupMesh);
			mesh->SetSkeletalMesh(SkeletalMesh);
			SetRootComponent(mesh);
			PickupRangeSphere->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
	*/

	// Don't need to search for the sphere component since it's already created it in the constructor.
	//PickupRangeSphere = FindComponentByClass<USphereComponent>();
	//PickupRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ABasePickup::OnOverlapBegin);

	if (PickupRangeSphere)
	{
		PickupRangeSphere->OnComponentBeginOverlap.AddDynamic(this,&ABasePickup::OnOverlapBegin);
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("PickupRangeSphere is invalid"));
		}
	}
}

#if WITH_EDITOR
void ABasePickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	PickupRangeSphere->SetSphereRadius(PickupRange);
}
#endif

void ABasePickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor != this) return;
}

// Called every frame
void ABasePickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABasePickup::OnInteract()
{
	//UE_LOG(LogTemp, Warning, TEXT("Interacted with pickup: %s"), *GetName());
	ABaseCharacter* Player = Cast<ABaseCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player not found!"));
		return;
	}
	if(!ItemDataAsset)
	{
		UE_LOG(LogTemp, Warning, TEXT("ItemDataAsset is null for pickup: %s"), *GetName());
		return;
	}

	if(bIsPickedUp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item already picked up: %s"), *ItemDataAsset->ItemName.ToString());
		return;
	}

	if (UInventoryComponent* Inventory = Player->FindComponentByClass<UInventoryComponent>())
	{
		if (Inventory->AddItem(ItemDataAsset))
		{
			bIsPickedUp = true;
			if (ItemDataAsset->bIsEquippable)
			{
				Inventory->OnWeaponPickedUp.Broadcast();
			}
			//UE_LOG(LogTemp, Warning, TEXT("Picked up item: %s"), *ItemDataAsset->ItemName.ToString());
			Destroy();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to pick up item: %s"), *ItemDataAsset->ItemName.ToString());
		}
	}
}

UInventoryItemDataAsset* ABasePickup::GetItemDataAsset()
{
	return ItemDataAsset;
}

void ABasePickup::SetItemDataAsset(UInventoryItemDataAsset* NewItemDataAsset)
{
	ItemDataAsset = NewItemDataAsset;
}

