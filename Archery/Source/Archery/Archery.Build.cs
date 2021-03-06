// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Archery : ModuleRules
{
	public Archery(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "NavigationSystem", "GameplayTasks" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");
		PrivateIncludePaths.Add("Archery");
		PrivateIncludePaths.Add("Archery/Enemy");
		PrivateIncludePaths.Add("Archery/ETC");
		PrivateIncludePaths.Add("Archery/Item");
		PrivateIncludePaths.Add("Archery/MainCharacter");
		PrivateIncludePaths.Add("Archery/Skill");
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

	}
}
