#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::AddItem(UItemDataAsset* Item)
{
	if (!Item)
	{
		return;
	}

	Items.Add(Item);
	Items.Sort([](const UItemDataAsset& A, const UItemDataAsset& B)
	{
		return A.ItemName.CompareTo(B.ItemName) < 0;
	});
	OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::RemoveItem(UItemDataAsset* Item)
{
	const int32 Index = Items.Find(Item);
	if (Index == INDEX_NONE)
	{
		return false;
	}

	Items.RemoveAt(Index);
	OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::HasItem(const UItemDataAsset* Item) const
{
	return Items.Contains(Item);
}

TArray<UItemDataAsset*> UInventoryComponent::GetItems() const
{
	TArray<UItemDataAsset*> Result;
	for (UItemDataAsset* Item : Items)
	{
		Result.Add(Item);
	}
	return Result;
}

TArray<UItemDataAsset*> UInventoryComponent::GetItemsOfType(EItemType Type) const
{
	TArray<UItemDataAsset*> Result;
	for (UItemDataAsset* Item : Items)
	{
		if (Item && Item->GetItemType() == Type)
		{
			Result.Add(Item);
		}
	}
	return Result;
}

int32 UInventoryComponent::GetTotalGoldValue() const
{
	int32 Total = 0;
	for (const UItemDataAsset* Item : Items)
	{
		if (Item)
		{
			Total += Item->GoldCost;
		}
	}
	return Total;
}
