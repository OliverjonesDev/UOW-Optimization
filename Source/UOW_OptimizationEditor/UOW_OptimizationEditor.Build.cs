using UnrealBuildTool;

public class UOW_OptimizationEditor : ModuleRules
{
	public UOW_OptimizationEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine" });

		PrivateDependencyModuleNames.AddRange(new string[] { "UnrealEd", "AssetRegistry", "Niagara", "UOW_Optimization" });
	}
}
