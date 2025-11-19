// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ZombieGround : ModuleRules
{
	public ZombieGround(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.AddRange(new string[] {
			"ZombieGround",
			"ZombieGround/Variant_Platforming",
			"ZombieGround/Variant_Platforming/Animation",
			"ZombieGround/Variant_Combat",
			"ZombieGround/Variant_Combat/AI",
			"ZombieGround/Variant_Combat/Animation",
			"ZombieGround/Variant_Combat/Gameplay",
			"ZombieGround/Variant_Combat/Interfaces",
			"ZombieGround/Variant_Combat/UI",
			"ZombieGround/Variant_SideScrolling",
			"ZombieGround/Variant_SideScrolling/AI",
			"ZombieGround/Variant_SideScrolling/Gameplay",
			"ZombieGround/Variant_SideScrolling/Interfaces",
			"ZombieGround/Variant_SideScrolling/UI"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
