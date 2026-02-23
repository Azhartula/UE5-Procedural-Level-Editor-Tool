// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Styling/AppStyle.h"

/**
 * Defines all UI commands for the Procedural Level Generator plugin.
 *
 * Each command is automatically registered with the input binding system and
 * can be mapped to toolbar buttons, menu items, or keyboard shortcuts.
 */
class FLevelGeneratorCommands : public TCommands<FLevelGeneratorCommands>
{
public:
	FLevelGeneratorCommands()
		: TCommands<FLevelGeneratorCommands>(
			TEXT("ProceduralLevelGenerator"),
			NSLOCTEXT("Contexts", "ProceduralLevelGenerator", "Procedural Level Generator Plugin"),
			NAME_None,
			FAppStyle::GetAppStyleSetName())
	{
	}

	/** Opens the main plugin panel. */
	TSharedPtr<FUICommandInfo> OpenPluginWindow;

	/** Triggers a new procedural generation pass with the current settings. */
	TSharedPtr<FUICommandInfo> GenerateLevel;

	/** Clears all spawned actors produced by the last generation pass. */
	TSharedPtr<FUICommandInfo> ClearLevel;

	// TCommands<> interface
	virtual void RegisterCommands() override;
};
