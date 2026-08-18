// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class ClockworkHorrorsEditorTarget : TargetRules
{
	public ClockworkHorrorsEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;

		ExtraModuleNames.AddRange( new string[] { "ClockworkHorrors" } );
	}
}
