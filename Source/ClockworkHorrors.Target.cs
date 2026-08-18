// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ClockworkHorrorsTarget : TargetRules
{
	public ClockworkHorrorsTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "ClockworkHorrors" } );
	}
}
