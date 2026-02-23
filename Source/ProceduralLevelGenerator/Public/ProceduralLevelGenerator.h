// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Framework/Commands/UICommandList.h"

class FToolBarBuilder;
class FMenuBuilder;

/**
 * Main module class for the Procedural Level Generator editor plugin.
 *
 * Registers the plugin tab, toolbar button, menu entries, and editor commands
 * when the editor starts. Unregisters them on shutdown.
 */
class FProceduralLevelGeneratorModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	/**
	 * Spawns the main dockable plugin tab.
	 * @param SpawnTabArgs - Arguments provided by the tab manager.
	 * @return The newly created dockable tab.
	 */
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	/** Registers editor commands and maps actions to the command list. */
	void RegisterCommands();

	/** Adds the plugin button to the main toolbar. */
	void AddToolbarExtension(FToolBarBuilder& Builder);

	/** Adds the plugin entry to the Window menu. */
	void AddMenuExtension(FMenuBuilder& Builder);

	/** Opens the plugin window; creates it if it doesn't exist yet. */
	void OpenPluginWindow();

	/** Shared command list used for all plugin UI actions. */
	TSharedPtr<FUICommandList> PluginCommands;
};
