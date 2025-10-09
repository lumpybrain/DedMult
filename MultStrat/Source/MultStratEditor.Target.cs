// Copyright Epic Games, Inc. All Rights Reserved.
// Note: Target files largely copied from Lyra Example project code. Keeping the Unreal copyright as a result.

using UnrealBuildTool;
using System.Collections.Generic;

// Target for the editor version of DedMult
public class MultStratEditorTarget : TargetRules
{
	public MultStratEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;

		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;

		MultStratTarget.ApplySharedLyraTargetSettings(this);
	}
}
