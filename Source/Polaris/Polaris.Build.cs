// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Polaris : ModuleRules
{
	public Polaris(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" });
	}
}
