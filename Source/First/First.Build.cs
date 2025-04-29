// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class First : ModuleRules
{
	public First(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "Slate", "SlateCore","Kismet"});

		PrivateDependencyModuleNames.AddRange(new string[] { "EnhancedInput", "AIModule", "GameplayTasks", "NavigationSystem" });
		
		PrivateDependencyModuleNames.AddRange(new string[] { "ChaosVehicles" });
		

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
