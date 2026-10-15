#include "Interaction/ItemPickup.h"
#include "Components/InventoryComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DataAssets/ItemDataAsset.h"
#include "Engine/StaticMesh.h"

AItemPickup::AItemPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
	MeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	MeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	MeshComponent->SetGenerateOverlapEvents(false);
	RootComponent = MeshComponent;
}

void AItemPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateMesh();
}

void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	BaseLocation = GetActorLocation();
	BaseRotation = GetActorRotation();
	BobPhase = FMath::FRand();
}

void AItemPickup::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunningTime += DeltaSeconds;

	if (!WasRecentlyRendered(0.2f))
	{
		return;
	}

	const float Bob = FMath::Sin((RunningTime * BobSpeed + BobPhase) * UE_TWO_PI) * BobHeight;
	SetActorLocationAndRotation(BaseLocation + FVector(0.f, 0.f, Bob), BaseRotation + FRotator(0.f, RunningTime * RotationSpeed, 0.f));
}

void AItemPickup::SetItem(UItemDataAsset* NewItem, int32 NewQuantity)
{
	Item = NewItem;
	Quantity = FMath::Max(1, NewQuantity);
	UpdateMesh();
}

void AItemPickup::UpdateMesh()
{
	UStaticMesh* Mesh = Item ? Item->Mesh.Get() : nullptr;
	MeshComponent->SetStaticMesh(Mesh);

	const float Radius = Mesh ? Mesh->GetBounds().SphereRadius : 0.f;
	MeshComponent->SetRelativeScale3D(FVector(Radius > 0.f ? DisplaySize / (Radius * 2.f) : 1.f));
}

void AItemPickup::Interact_Implementation(AActor* Interactor)
{
	UInventoryComponent* Inventory = Interactor ? Interactor->FindComponentByClass<UInventoryComponent>() : nullptr;
	if (!Inventory || !Item)
	{
		return;
	}

	for (int32 Index = 0; Index < Quantity; Index++)
	{
		Inventory->AddItem(Item);
	}

	Destroy();
}

FText AItemPickup::GetInteractionPrompt_Implementation() const
{
	return Item ? FText::Format(FText::FromString(TEXT("Pick up {0}")), Item->ItemName) : FText::GetEmpty();
}
