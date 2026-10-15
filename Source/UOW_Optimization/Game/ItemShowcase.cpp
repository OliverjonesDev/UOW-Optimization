#include "Game/ItemShowcase.h"
#include "DataAssets/ItemDataAsset.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/PointLightComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "DrawDebugHelpers.h"

AItemShowcase::AItemShowcase()
{
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void AItemShowcase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ClearDisplays();

	if (bAutoPopulate)
	{
		PopulateFromAssetRegistry();
	}

	for (UItemDataAsset* Item : Items)
	{
		if (Item && Item->Mesh)
		{
			BuildDisplay(Item);
		}
	}
}

void AItemShowcase::ClearDisplays()
{
	for (UStaticMeshComponent* Mesh : DisplayMeshes)
	{
		if (IsValid(Mesh))
		{
			Mesh->DestroyComponent();
		}
	}

	for (UTextRenderComponent* Label : Labels)
	{
		if (IsValid(Label))
		{
			Label->DestroyComponent();
		}
	}

	for (UPointLightComponent* Light : Lights)
	{
		if (IsValid(Light))
		{
			Light->DestroyComponent();
		}
	}

	DisplayedItems.Reset();
	DisplayMeshes.Reset();
	Labels.Reset();
	Lights.Reset();
	DisplayMaterials.Reset();
}

void AItemShowcase::PopulateFromAssetRegistry()
{
	Items.Reset();

	TArray<FAssetData> Assets;
	IAssetRegistry::GetChecked().GetAssetsByPath(FName(*AutoPopulatePath), Assets, true);

	for (const FAssetData& Asset : Assets)
	{
		if (Asset.IsInstanceOf(UItemDataAsset::StaticClass()))
		{
			Items.Add(Cast<UItemDataAsset>(Asset.GetAsset()));
		}
	}
}

void AItemShowcase::BuildDisplay(UItemDataAsset* Item)
{
	const int32 Index = DisplayedItems.Num();
	const FVector Location((Index % Columns) * Spacing, (Index / Columns) * Spacing, DisplaySize);
	const float Radius = Item->Mesh->GetBounds().SphereRadius;
	const float Scale = Radius > 0.f ? DisplaySize / (Radius * 2.f) : 1.f;

	UStaticMeshComponent* Mesh = NewObject<UStaticMeshComponent>(this, NAME_None, RF_Transactional);
	Mesh->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetMobility(EComponentMobility::Movable);
	Mesh->SetStaticMesh(Item->Mesh);
	Mesh->SetRelativeLocation(Location);
	Mesh->SetRelativeScale3D(FVector(Scale));
	Mesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->RegisterComponent();

	for (int32 MaterialIndex = 0; MaterialIndex < Mesh->GetNumMaterials(); MaterialIndex++)
	{
		if (UMaterialInstanceDynamic* Material = Mesh->CreateAndSetMaterialInstanceDynamic(MaterialIndex))
		{
			Material->SetVectorParameterValue(TEXT("Tint"), FLinearColor::MakeRandomColor());
			DisplayMaterials.Add(Material);
		}
	}

	UTextRenderComponent* Label = NewObject<UTextRenderComponent>(this, NAME_None, RF_Transactional);
	Label->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	Label->SetupAttachment(RootComponent);
	Label->SetRelativeLocation(Location + FVector(0.f, 0.f, DisplaySize));
	Label->SetHorizontalAlignment(EHTA_Center);
	Label->SetWorldSize(DisplaySize * 0.2f);
	Label->RegisterComponent();

	UPointLightComponent* Light = NewObject<UPointLightComponent>(this, NAME_None, RF_Transactional);
	Light->CreationMethod = EComponentCreationMethod::UserConstructionScript;
	Light->SetupAttachment(RootComponent);
	Light->SetMobility(EComponentMobility::Movable);
	Light->SetRelativeLocation(Location + FVector(0.f, 0.f, DisplaySize * 1.5f));
	Light->SetIntensity(5000.f);
	Light->SetAttenuationRadius(Spacing);
	Light->SetCastShadows(true);
	Light->RegisterComponent();

	DisplayedItems.Add(Item);
	DisplayMeshes.Add(Mesh);
	Labels.Add(Label);
	Lights.Add(Light);
}

void AItemShowcase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	RunningTime += DeltaSeconds;

	for (int32 Index = 0; Index < DisplayMeshes.Num(); Index++)
	{
		UStaticMeshComponent* Mesh = DisplayMeshes[Index];
		UItemDataAsset* Item = DisplayedItems[Index];
		if (!Mesh || !Item)
		{
			continue;
		}

		Mesh->SetRelativeRotation(FRotator(0.f, RunningTime * RotationSpeed + Index * 15.f, 0.f));
		Labels[Index]->SetText(FText::Format(FText::FromString(TEXT("{0}\n{1}g")), Item->ItemName, FText::AsNumber(Item->GoldCost)));

		if (bShowBounds)
		{
			DrawDebugBox(GetWorld(), Mesh->Bounds.Origin, Mesh->Bounds.BoxExtent, FColor::Cyan, true);
		}
	}

	for (UMaterialInstanceDynamic* Material : DisplayMaterials)
	{
		Material->SetScalarParameterValue(TEXT("Glow"), 0.5f + 0.5f * FMath::Sin(RunningTime * 2.f));
	}
}
