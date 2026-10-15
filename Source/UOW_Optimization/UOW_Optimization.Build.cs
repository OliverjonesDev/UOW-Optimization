// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UOW_Optimization : ModuleRules
{
	public UOW_Optimization(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "Niagara", "AIModule", "GameplayTasks", "AssetRegistry" });

		PublicIncludePaths.Add(ModuleDirectory);
	}
}
