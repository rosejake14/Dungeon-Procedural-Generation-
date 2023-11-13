// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PCGTesting : ModuleRules
{
	public PCGTesting(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
