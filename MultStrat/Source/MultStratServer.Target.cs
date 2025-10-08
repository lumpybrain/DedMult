// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

[SupportedPlatforms(UnrealPlatformClass.Server)]
public class MultStratServerTarget : TargetRules
{
	public MultStratServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		//ExtraModuleNames.AddRange(new string[] { "MultStrat" });

		MultStratTarget.ApplySharedLyraTargetSettings(this);

		bUseChecksInShipping = true;
	}
}
