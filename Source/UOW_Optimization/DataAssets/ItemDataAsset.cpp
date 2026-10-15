#include "DataAssets/ItemDataAsset.h"

FPrimaryAssetId UItemDataAsset::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(TEXT("Item"), GetFName());
}
