// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MultStratEditor : ModuleRules
{
	public MultStratEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
		new string[]
		{
			"MultStratEditor/Public"
		});

		PrivateIncludePaths.AddRange(
		new string[]
		{
			"MultStratEditor/Private"
		});

		PublicDependencyModuleNames.AddRange(new string[] { "MultStrat", "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new string[] { "ComponentVisualizers", "UnrealEd" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
