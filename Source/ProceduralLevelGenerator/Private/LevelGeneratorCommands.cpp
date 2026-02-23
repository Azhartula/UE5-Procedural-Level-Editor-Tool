// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#include "LevelGeneratorCommands.h"

#define LOCTEXT_NAMESPACE "FLevelGeneratorCommands"

void FLevelGeneratorCommands::RegisterCommands()
{
	UI_COMMAND(
		OpenPluginWindow,
		"Procedural Level Generator",
		"Opens the Procedural Level Generator panel",
		EUserInterfaceActionType::Button,
		FInputChord());

	UI_COMMAND(
		GenerateLevel,
		"Generate Level",
		"Runs the Wave Function Collapse algorithm and spawns actors in the current level",
		EUserInterfaceActionType::Button,
		FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::G));

	UI_COMMAND(
		ClearLevel,
		"Clear Generated Level",
		"Removes all actors spawned by the last generation pass",
		EUserInterfaceActionType::Button,
		FInputChord(EModifierKey::Control | EModifierKey::Shift, EKeys::C));
}

#undef LOCTEXT_NAMESPACE
