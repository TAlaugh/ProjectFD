// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectFD : ModuleRules
{
	public ProjectFD(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "NetCore", "Networking", "InputCore", "EnhancedInput", "GameplayAbilities", "AIModule", "Slate", "SlateCore", "NavigationSystem" });

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "GameplayTags", "GameplayTasks", "UMG", "AnimGraphRuntime", "MotionWarping", "Niagara", "RD", "CableComponent" });

		PublicDefinitions.Add("NOMINMAX");
		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
