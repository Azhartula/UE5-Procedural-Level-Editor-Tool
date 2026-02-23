// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#include "ProceduralLevelGenerator.h"
#include "LevelGeneratorCommands.h"
#include "SLevelGeneratorWidget.h"

#include "Framework/Docking/TabManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Framework/Application/SlateApplication.h"
#include "Interfaces/IMainFrameModule.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

static const FName ProceduralLevelGeneratorTabName("ProceduralLevelGenerator");

#define LOCTEXT_NAMESPACE "FProceduralLevelGeneratorModule"

void FProceduralLevelGeneratorModule::StartupModule()
{
	RegisterCommands();

	// Register the dockable tab so the tab manager can spawn it on demand.
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
		ProceduralLevelGeneratorTabName,
		FOnSpawnTab::CreateRaw(this, &FProceduralLevelGeneratorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("ProceduralLevelGeneratorTabTitle", "Procedural Level Generator"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Extend the Level Editor toolbar.
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension(
			"Settings",
			EExtensionHook::After,
			PluginCommands,
			FToolBarExtensionDelegate::CreateRaw(this, &FProceduralLevelGeneratorModule::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	// Extend the Window menu.
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender);
		MenuExtender->AddMenuExtension(
			"WindowLayout",
			EExtensionHook::After,
			PluginCommands,
			FMenuExtensionDelegate::CreateRaw(this, &FProceduralLevelGeneratorModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}
}

void FProceduralLevelGeneratorModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);

	FLevelGeneratorCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProceduralLevelGeneratorTabName);
}

TSharedRef<SDockTab> FProceduralLevelGeneratorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SLevelGeneratorWidget)
		];
}

void FProceduralLevelGeneratorModule::RegisterCommands()
{
	FLevelGeneratorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FLevelGeneratorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FProceduralLevelGeneratorModule::OpenPluginWindow),
		FCanExecuteAction());
}

void FProceduralLevelGeneratorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FLevelGeneratorCommands::Get().OpenPluginWindow);
}

void FProceduralLevelGeneratorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FLevelGeneratorCommands::Get().OpenPluginWindow);
}

void FProceduralLevelGeneratorModule::OpenPluginWindow()
{
	FGlobalTabmanager::Get()->TryInvokeTab(ProceduralLevelGeneratorTabName);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FProceduralLevelGeneratorModule, ProceduralLevelGenerator)
