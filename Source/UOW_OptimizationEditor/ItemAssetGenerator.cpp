#include "ItemAssetGenerator.h"
#include "DataAssets/WeaponItemDataAsset.h"
#include "DataAssets/PotionItemDataAsset.h"
#include "DataAssets/IngredientItemDataAsset.h"
#include "DataAssets/MiscItemDataAsset.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "NiagaraSystem.h"
#include "HAL/FileManager.h"
#include "Misc/PackageName.h"
#include "ObjectTools.h"
#include "UObject/SavePackage.h"

static const FString GeneratedPath = TEXT("/Game/UOW_Custom/Generated");
static constexpr int32 AssetPoolSize = 40;

static const TCHAR* Adjectives[] = { TEXT("Ancient"), TEXT("Rusty"), TEXT("Glowing"), TEXT("Cursed"), TEXT("Royal"), TEXT("Bitter"), TEXT("Shadow"), TEXT("Frozen"), TEXT("Burning"), TEXT("Gilded"), TEXT("Hollow"), TEXT("Wild") };
static const TCHAR* WeaponNouns[] = { TEXT("Sword"), TEXT("Axe"), TEXT("Mace"), TEXT("Spear"), TEXT("Dagger"), TEXT("Halberd") };
static const TCHAR* PotionNouns[] = { TEXT("Elixir"), TEXT("Tonic"), TEXT("Draught"), TEXT("Brew"), TEXT("Philter") };
static const TCHAR* IngredientNouns[] = { TEXT("Root"), TEXT("Petal"), TEXT("Mushroom"), TEXT("Herb"), TEXT("Crystal"), TEXT("Moss") };
static const TCHAR* MiscNouns[] = { TEXT("Key"), TEXT("Coin"), TEXT("Relic"), TEXT("Scroll"), TEXT("Gem"), TEXT("Idol") };

template<typename ArrayType>
static const TCHAR* PickWord(const ArrayType& Words, FRandomStream& Stream)
{
	return Words[Stream.RandRange(0, UE_ARRAY_COUNT(Words) - 1)];
}

static IAssetRegistry& GetAssetRegistry()
{
	IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	AssetRegistry.SearchAllAssets(true);
	return AssetRegistry;
}

static TArray<FSoftObjectPath> FindLargestAssets(UClass* Class, bool bGameContentOnly)
{
	TArray<FAssetData> Assets;
	GetAssetRegistry().GetAssetsByClass(Class->GetClassPathName(), Assets);

	TArray<TPair<int64, FSoftObjectPath>> SizedAssets;
	for (const FAssetData& Asset : Assets)
	{
		const FString PackageName = Asset.PackageName.ToString();
		if ((bGameContentOnly && !PackageName.StartsWith(TEXT("/Game/"))) || PackageName.StartsWith(GeneratedPath))
		{
			continue;
		}

		FString FileName;
		if (FPackageName::DoesPackageExist(PackageName, &FileName))
		{
			SizedAssets.Emplace(IFileManager::Get().FileSize(*FileName), Asset.GetSoftObjectPath());
		}
	}

	SizedAssets.Sort([](const TPair<int64, FSoftObjectPath>& A, const TPair<int64, FSoftObjectPath>& B)
	{
		return A.Key > B.Key;
	});

	TArray<FSoftObjectPath> Result;
	for (int32 Index = 0; Index < SizedAssets.Num() && Index < AssetPoolSize; Index++)
	{
		Result.Add(SizedAssets[Index].Value);
	}
	return Result;
}

template<typename T>
static T* PickAsset(const TArray<FSoftObjectPath>& Pool, FRandomStream& Stream)
{
	if (Pool.IsEmpty())
	{
		return nullptr;
	}

	return Cast<T>(Pool[Stream.RandRange(0, Pool.Num() - 1)].TryLoad());
}

static UItemDataAsset* CreateItem(UClass* Class, const FString& AssetName)
{
	UPackage* Package = CreatePackage(*(GeneratedPath / AssetName));
	UItemDataAsset* Item = NewObject<UItemDataAsset>(Package, Class, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
	FAssetRegistryModule::AssetCreated(Item);
	return Item;
}

static void SaveItem(UItemDataAsset* Item)
{
	UPackage* Package = Item->GetPackage();
	Package->MarkPackageDirty();

	const FString FileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;
	UPackage::SavePackage(Package, Item, *FileName, SaveArgs);
}

void FItemAssetGenerator::ClearGenerated()
{
	TArray<FAssetData> Assets;
	GetAssetRegistry().GetAssetsByPath(FName(*GeneratedPath), Assets, true);

	TArray<UObject*> Objects;
	for (const FAssetData& Asset : Assets)
	{
		if (UObject* Object = Asset.GetAsset())
		{
			Objects.Add(Object);
		}
	}

	if (!Objects.IsEmpty())
	{
		ObjectTools::ForceDeleteObjects(Objects, false);
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}

	UE_LOG(LogTemp, Display, TEXT("Deleted %d generated items from %s"), Objects.Num(), *GeneratedPath);
}

void FItemAssetGenerator::Generate(int32 Count, int32 Seed, int32 LinksPerItem)
{
	ClearGenerated();

	FRandomStream Stream(Seed);

	const TArray<FSoftObjectPath> Textures = FindLargestAssets(UTexture2D::StaticClass(), true);
	const TArray<FSoftObjectPath> Meshes = FindLargestAssets(UStaticMesh::StaticClass(), true);
	TArray<FSoftObjectPath> ParticleSystems = FindLargestAssets(UNiagaraSystem::StaticClass(), true);
	if (ParticleSystems.IsEmpty())
	{
		ParticleSystems = FindLargestAssets(UNiagaraSystem::StaticClass(), false);
	}

	UE_LOG(LogTemp, Display, TEXT("Item generator pools: %d textures, %d meshes, %d particle systems"), Textures.Num(), Meshes.Num(), ParticleSystems.Num());

	TArray<UItemDataAsset*> Items;
	TArray<UPotionItemDataAsset*> Potions;
	TArray<UIngredientItemDataAsset*> Ingredients;

	for (int32 Index = 0; Index < Count; Index++)
	{
		const FString Adjective = PickWord(Adjectives, Stream);
		UItemDataAsset* Item = nullptr;
		FString Noun;

		switch (Stream.RandRange(0, 3))
		{
		case 0:
		{
			Noun = PickWord(WeaponNouns, Stream);
			UWeaponItemDataAsset* Weapon = Cast<UWeaponItemDataAsset>(CreateItem(UWeaponItemDataAsset::StaticClass(), FString::Printf(TEXT("DA_Weapon_%03d"), Index)));
			Weapon->Damage = Stream.FRandRange(5.f, 80.f);
			Weapon->Range = Stream.FRandRange(100.f, 400.f);
			Weapon->GoldCost = Stream.RandRange(50, 1000);
			Item = Weapon;
			break;
		}
		case 1:
		{
			Noun = PickWord(PotionNouns, Stream);
			UPotionItemDataAsset* Potion = Cast<UPotionItemDataAsset>(CreateItem(UPotionItemDataAsset::StaticClass(), FString::Printf(TEXT("DA_Potion_%03d"), Index)));
			Potion->HealthRestore = Stream.FRand() < 0.5f ? Stream.FRandRange(10.f, 100.f) : 0.f;
			Potion->StaminaRestore = Stream.FRand() < 0.5f ? Stream.FRandRange(10.f, 100.f) : 0.f;
			Potion->ManaRestore = Stream.FRand() < 0.5f ? Stream.FRandRange(10.f, 100.f) : 0.f;
			Potion->Duration = Stream.FRand() < 0.3f ? Stream.FRandRange(5.f, 30.f) : 0.f;
			Potion->MaxStackSize = Stream.RandRange(5, 20);
			Potion->GoldCost = Stream.RandRange(10, 200);
			Potions.Add(Potion);
			Item = Potion;
			break;
		}
		case 2:
		{
			Noun = PickWord(IngredientNouns, Stream);
			UIngredientItemDataAsset* Ingredient = Cast<UIngredientItemDataAsset>(CreateItem(UIngredientItemDataAsset::StaticClass(), FString::Printf(TEXT("DA_Ingredient_%03d"), Index)));
			Ingredient->MaxStackSize = Stream.RandRange(10, 99);
			Ingredient->Potency = Stream.FRandRange(0.1f, 5.f);
			Ingredient->GoldCost = Stream.RandRange(1, 50);
			Ingredients.Add(Ingredient);
			Item = Ingredient;
			break;
		}
		default:
		{
			Noun = PickWord(MiscNouns, Stream);
			UMiscItemDataAsset* Misc = Cast<UMiscItemDataAsset>(CreateItem(UMiscItemDataAsset::StaticClass(), FString::Printf(TEXT("DA_Misc_%03d"), Index)));
			Misc->MaxStackSize = Stream.RandRange(1, 99);
			Misc->bIsQuestItem = Stream.FRand() < 0.1f;
			Misc->GoldCost = Stream.RandRange(1, 500);
			Item = Misc;
			break;
		}
		}

		Item->ItemName = FText::FromString(FString::Printf(TEXT("%s %s"), *Adjective, *Noun));
		Item->Description = FText::FromString(FString::Printf(TEXT("A %s %s of unknown origin."), *Adjective.ToLower(), *Noun.ToLower()));
		Item->Icon = PickAsset<UTexture2D>(Textures, Stream);
		Item->Mesh = PickAsset<UStaticMesh>(Meshes, Stream);
		Item->ParticleSystem = PickAsset<UNiagaraSystem>(ParticleSystems, Stream);
		Items.Add(Item);
	}

	for (UItemDataAsset* Item : Items)
	{
		for (int32 Link = 0; Link < LinksPerItem && Items.Num() > 1; Link++)
		{
			UItemDataAsset* Other = Items[Stream.RandRange(0, Items.Num() - 1)];
			if (Other != Item)
			{
				Item->RelatedItems.AddUnique(Other);
			}
		}
	}

	for (UIngredientItemDataAsset* Ingredient : Ingredients)
	{
		const int32 PotionLinks = FMath::Min(Potions.Num(), Stream.RandRange(1, 3));
		for (int32 Link = 0; Link < PotionLinks; Link++)
		{
			Ingredient->UsedInPotions.AddUnique(Potions[Stream.RandRange(0, Potions.Num() - 1)]);
		}
	}

	for (UItemDataAsset* Item : Items)
	{
		SaveItem(Item);
	}

	UE_LOG(LogTemp, Display, TEXT("Generated %d items in %s (seed %d)"), Items.Num(), *GeneratedPath, Seed);
}

static FAutoConsoleCommand GenerateItemsCommand(
	TEXT("Items.Generate"),
	TEXT("Items.Generate [Count=100] [Seed=random] [LinksPerItem=3]. Replaces all items in /Game/UOW_Custom/Generated with random linked item data assets."),
	FConsoleCommandWithArgsDelegate::CreateLambda([](const TArray<FString>& Args)
	{
		const int32 Count = Args.IsValidIndex(0) ? FCString::Atoi(*Args[0]) : 100;
		const int32 Seed = Args.IsValidIndex(1) ? FCString::Atoi(*Args[1]) : FMath::Rand();
		const int32 LinksPerItem = Args.IsValidIndex(2) ? FCString::Atoi(*Args[2]) : 3;
		FItemAssetGenerator::Generate(FMath::Max(1, Count), Seed, FMath::Max(0, LinksPerItem));
	}));

static FAutoConsoleCommand ClearItemsCommand(
	TEXT("Items.ClearGenerated"),
	TEXT("Deletes all items in /Game/UOW_Custom/Generated."),
	FConsoleCommandDelegate::CreateStatic(&FItemAssetGenerator::ClearGenerated));
