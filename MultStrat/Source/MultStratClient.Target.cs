// Copyright Epic Games, Inc. All Rights Reserved.
// DedMult Portfolio Piece Code

// Note: Target files largely copied from Lyra Example project code. Keeping the Unreal copyright as a result.

using UnrealBuildTool;
using System.Collections.Generic;

// Target for the client version of dedmult, for use when connecting to a dedicated or a listen server
public class MultStratClientTarget : TargetRules
{
	public MultStratClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		MultStratTarget.ApplySharedLyraTargetSettings(this);
	}
}
