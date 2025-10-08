// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MultStratClientTarget : TargetRules
{
	public MultStratClientTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;

		//ExtraModuleNames.AddRange(new string[] { "MultStratGame" });

		MultStratTarget.ApplySharedLyraTargetSettings(this);
	}
}
