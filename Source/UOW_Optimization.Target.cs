// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class UOW_OptimizationTarget : TargetRules
{
	public UOW_OptimizationTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V7;
		ExtraModuleNames.Add("UOW_Optimization");
	}
}
