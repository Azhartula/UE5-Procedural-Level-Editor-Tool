// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

using UnrealBuildTool;

public class ProceduralLevelGenerator : ModuleRules
{
	public ProceduralLevelGenerator(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
			}
		);

		PrivateIncludePaths.AddRange(
			new string[] {
			}
		);

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"EditorWidgets",
				"LevelEditor",
				"ToolMenus",
				"WorkspaceMenuStructure",
				"PropertyEditor",
				"AssetTools",
				"ContentBrowser",
				"Projects",
			}
		);
	}
}
