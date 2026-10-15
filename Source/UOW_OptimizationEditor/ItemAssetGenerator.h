#pragma once

#include "CoreMinimal.h"

class FItemAssetGenerator
{
public:
	static void Generate(int32 Count, int32 Seed, int32 LinksPerItem);
	static void ClearGenerated();
};
