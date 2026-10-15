#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "HAL/PlatformMemory.h"
#include "Misc/PackageName.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectHash.h"
#include "UObject/Package.h"
#include "DataAssets/ItemDataAsset.h"

#if !UE_BUILD_SHIPPING

struct FClassLoadStats
{
	int32 Count = 0;
	int64 Bytes = 0;
};

static TSet<FName> GetLoadedPackageNames()
{
	TSet<FName> Names;
	for (TObjectIterator<UPackage> It; It; ++It)
	{
		Names.Add(It->GetFName());
	}
	return Names;
}

static double ToMegabytes(int64 Bytes)
{
	return Bytes / (1024.0 * 1024.0);
}

static void MeasureItemLoad(const TArray<FString>& Args, UWorld* World, FOutputDevice& Ar)
{
	if (Args.IsEmpty())
	{
		Ar.Log(TEXT("Usage: Items.MeasureLoad <AssetPath>   e.g. Items.MeasureLoad /Game/UOW_Custom/DataAssets/DA_Ingre_Flowers"));
		return;
	}

	FString Path = Args[0];
	if (!Path.Contains(TEXT(".")))
	{
		Path += TEXT(".") + FPackageName::GetShortName(Path);
	}

	const FSoftObjectPath AssetPath(Path);
	if (AssetPath.ResolveObject())
	{
		Ar.Logf(TEXT("%s is already in memory, so nothing new would load. Run 'obj gc' and retry, or launch as Standalone Game."), *Path);
		return;
	}

	const TSet<FName> PackagesBefore = GetLoadedPackageNames();
	const int32 ObjectsBefore = GUObjectArray.GetObjectArrayNumMinusAvailable();
	const int64 MemoryBefore = FPlatformMemory::GetStats().UsedPhysical;
	const double StartTime = FPlatformTime::Seconds();

	UObject* Asset = AssetPath.TryLoad();
	FlushAsyncLoading();

	const double LoadMilliseconds = (FPlatformTime::Seconds() - StartTime) * 1000.0;
	const int64 MemoryAfter = FPlatformMemory::GetStats().UsedPhysical;

	if (!Asset)
	{
		Ar.Logf(TEXT("Failed to load %s"), *Path);
		return;
	}

	TMap<UClass*, FClassLoadStats> ClassStats;
	int32 NewPackages = 0;
	int32 ItemsLoaded = 0;
	int64 TotalBytes = 0;

	for (TObjectIterator<UPackage> It; It; ++It)
	{
		if (PackagesBefore.Contains(It->GetFName()))
		{
			continue;
		}

		NewPackages++;
		ForEachObjectWithPackage(*It, [&](UObject* Object)
		{
			const int64 Bytes = Object->GetResourceSizeBytes(EResourceSizeMode::Exclusive);
			FClassLoadStats& Stats = ClassStats.FindOrAdd(Object->GetClass());
			Stats.Count++;
			Stats.Bytes += Bytes;
			TotalBytes += Bytes;

			if (Object->IsA<UItemDataAsset>())
			{
				ItemsLoaded++;
			}
			return true;
		});
	}

	ClassStats.ValueSort([](const FClassLoadStats& A, const FClassLoadStats& B)
	{
		return A.Bytes > B.Bytes;
	});

	Ar.Logf(TEXT("Items.MeasureLoad %s"), *Path);
	Ar.Logf(TEXT("  Load time:          %.1f ms"), LoadMilliseconds);
	Ar.Logf(TEXT("  Packages loaded:    %d"), NewPackages);
	Ar.Logf(TEXT("  Objects created:    %d"), GUObjectArray.GetObjectArrayNumMinusAvailable() - ObjectsBefore);
	Ar.Logf(TEXT("  Item assets loaded: %d"), ItemsLoaded);
	Ar.Logf(TEXT("  Resource memory:    %.2f MB"), ToMegabytes(TotalBytes));
	Ar.Logf(TEXT("  Process memory:     %+.2f MB"), ToMegabytes(MemoryAfter - MemoryBefore));
	Ar.Log(TEXT("  Largest classes:"));

	int32 Shown = 0;
	for (const TPair<UClass*, FClassLoadStats>& Pair : ClassStats)
	{
		if (Shown++ >= 10)
		{
			break;
		}

		Ar.Logf(TEXT("    %-32s %6d objects %10.2f MB"), *Pair.Key->GetName(), Pair.Value.Count, ToMegabytes(Pair.Value.Bytes));
	}
}

static FAutoConsoleCommand MeasureItemLoadCommand(
	TEXT("Items.MeasureLoad"),
	TEXT("Items.MeasureLoad <AssetPath>. Loads one asset and reports everything that was pulled into memory with it."),
	FConsoleCommandWithWorldArgsAndOutputDeviceDelegate::CreateStatic(&MeasureItemLoad));

#endif
