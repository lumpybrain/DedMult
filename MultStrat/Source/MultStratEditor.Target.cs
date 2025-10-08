// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultStratEditorTarget : TargetRules
{
	public MultStratEditorTarget( TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_6;
		//ExtraModuleNames.Add("MultStrat");
		MultStratTarget.ApplySharedLyraTargetSettings(this);
	}
}
