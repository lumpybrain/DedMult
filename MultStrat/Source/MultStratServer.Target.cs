// Copyright Epic Games, Inc. All Rights Reserved.
// DedMult Portfolio Piece Code

// Note: Target files largely copied from Lyra Example project code. Keeping the Unreal copyright as a result.

using UnrealBuildTool;
using System.Collections.Generic;

// Target for the dedicated server version of Dedmult
[SupportedPlatforms(UnrealPlatformClass.Server)]
public class MultStratServerTarget : TargetRules
{
	public MultStratServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		MultStratTarget.ApplySharedLyraTargetSettings(this);

		bUseChecksInShipping = true;
	}
}
