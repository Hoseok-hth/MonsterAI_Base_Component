// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class test1 : ModuleRules
{
	public test1(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			//default
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput",
			
			//AI
			"AIModule",
			
			//Navigation
			"NavigationSystem",
			
			//Tags
			"GameplayTags",
		});

		PublicIncludePaths.AddRange(new string[] 
		{ 
			"test1" 
		});
		PrivateDependencyModuleNames.AddRange(new string[] { "AIModule" });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
