// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UOW_OptimizationEditorTarget : TargetRules
{
	public UOW_OptimizationEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.AddRange(new string[] { "UOW_Optimization", "UOW_OptimizationEditor" });
	}
}
