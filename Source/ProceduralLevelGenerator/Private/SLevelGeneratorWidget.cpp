// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#include "SLevelGeneratorWidget.h"
#include "WaveFunctionCollapse.h"
#include "GenerationRuleSet.h"

#include "Editor.h"
#include "EditorStyleSet.h"
#include "Engine/World.h"
#include "PropertyCustomizationHelpers.h"
#include "Styling/AppStyle.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SLevelGeneratorWidget"

void SLevelGeneratorWidget::Construct(const FArguments& InArgs)
{
	GridSize = InArgs._GridSize;
	RandomSeed = InArgs._RandomSeed;
	RuleSet = InArgs._RuleSet;
	OnGenerationCompleteDelegate = InArgs._OnGenerationComplete;

	ChildSlot
	[
		SNew(SScrollBox)
		+ SScrollBox::Slot()
		[
			SNew(SVerticalBox)

			// ---- Header ----
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 8.0f, 8.0f, 4.0f)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Title", "Procedural Level Generator"))
				.TextStyle(FAppStyle::Get(), "LargeText")
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 0.0f)
			[
				SNew(SSeparator)
			]

			// ---- Parameters Panel ----
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 4.0f)
			[
				BuildParametersPanel()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 0.0f)
			[
				SNew(SSeparator)
			]

			// ---- Rule Set Browser ----
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 4.0f)
			[
				BuildRuleSetPanel()
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 0.0f)
			[
				SNew(SSeparator)
			]

			// ---- Action Buttons ----
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 8.0f)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(0.0f, 0.0f, 4.0f, 0.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(this, &SLevelGeneratorWidget::GetGenerateButtonLabel)
					.IsEnabled(this, &SLevelGeneratorWidget::IsGenerateEnabled)
					.OnClicked(this, &SLevelGeneratorWidget::OnGenerateClicked)
					.ToolTipText(LOCTEXT("GenerateTooltip",
						"Run the Wave Function Collapse algorithm and spawn actors in the current level"))
				]
				+ SHorizontalBox::Slot()
				.FillWidth(1.0f)
				.Padding(4.0f, 0.0f, 0.0f, 0.0f)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.Text(LOCTEXT("ClearButton", "Clear"))
					.OnClicked(this, &SLevelGeneratorWidget::OnClearClicked)
					.ToolTipText(LOCTEXT("ClearTooltip",
						"Remove all actors spawned by the last generation pass"))
				]
			]

			// ---- Statistics Panel ----
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 0.0f)
			[
				SNew(SSeparator)
			]

			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f, 4.0f)
			[
				BuildStatisticsPanel()
			]
		]
	];
}

// -----------------------------------------------------------------------------
// Panel builders
// -----------------------------------------------------------------------------

TSharedRef<SWidget> SLevelGeneratorWidget::BuildParametersPanel()
{
	return SNew(SVerticalBox)

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("ParametersHeader", "Parameters"))
		.TextStyle(FAppStyle::Get(), "NormalText.Important")
	]

	// Grid Width
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.4f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("GridWidth", "Grid Width"))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.6f)
		[
			SNew(SSpinBox<int32>)
			.MinValue(1)
			.MaxValue(100)
			.Value(this, &SLevelGeneratorWidget::GetGridSizeX)
			.OnValueChanged(this, &SLevelGeneratorWidget::OnGridSizeXChanged)
		]
	]

	// Grid Height
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.4f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("GridHeight", "Grid Height"))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.6f)
		[
			SNew(SSpinBox<int32>)
			.MinValue(1)
			.MaxValue(100)
			.Value(this, &SLevelGeneratorWidget::GetGridSizeY)
			.OnValueChanged(this, &SLevelGeneratorWidget::OnGridSizeYChanged)
		]
	]

	// Random Seed
	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(0.4f)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock).Text(LOCTEXT("RandomSeed", "Random Seed"))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(0.6f)
		[
			SNew(SSpinBox<int32>)
			.MinValue(0)
			.MaxValue(TNumericLimits<int32>::Max())
			.Value(this, &SLevelGeneratorWidget::GetSeedValue)
			.OnValueChanged(this, &SLevelGeneratorWidget::OnSeedChanged)
			.ToolTipText(LOCTEXT("SeedTooltip", "Set to 0 to use a random seed each run"))
		]
	];
}

TSharedRef<SWidget> SLevelGeneratorWidget::BuildRuleSetPanel()
{
	return SNew(SVerticalBox)

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("RuleSetHeader", "Rule Set"))
		.TextStyle(FAppStyle::Get(), "NormalText.Important")
	]

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(SObjectPropertyEntryBox)
		.AllowedClass(UGenerationRuleSet::StaticClass())
		.ObjectPath_Lambda([this]() -> FString
		{
			if (RuleSet.IsValid())
			{
				return RuleSet->GetPathName();
			}
			return FString();
		})
		.OnObjectChanged_Lambda([this](const FAssetData& AssetData)
		{
			RuleSet = Cast<UGenerationRuleSet>(AssetData.GetAsset());
		})
	];
}

TSharedRef<SWidget> SLevelGeneratorWidget::BuildStatisticsPanel()
{
	return SNew(SVerticalBox)

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(STextBlock)
		.Text(LOCTEXT("StatsHeader", "Statistics"))
		.TextStyle(FAppStyle::Get(), "NormalText.Important")
	]

	+ SVerticalBox::Slot()
	.AutoHeight()
	.Padding(0.0f, 2.0f)
	[
		SNew(STextBlock)
		.Text_Lambda([this]() -> FText
		{
			if (LastGenerationTime <= 0.0)
			{
				return LOCTEXT("StatsNoRun", "No generation run yet.");
			}
			return FText::Format(
				LOCTEXT("StatsLastRun", "Last generation: {0} ms  |  Grid: {1}x{2}"),
				FText::AsNumber(static_cast<int32>(LastGenerationTime * 1000.0)),
				FText::AsNumber(GridSize.X),
				FText::AsNumber(GridSize.Y));
		})
	];
}

// -----------------------------------------------------------------------------
// Button handlers
// -----------------------------------------------------------------------------

FReply SLevelGeneratorWidget::OnGenerateClicked()
{
	if (bIsGenerating)
	{
		return FReply::Handled();
	}

	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (!World)
	{
		return FReply::Handled();
	}

	if (!RuleSet.IsValid() || RuleSet->TileModules.Num() == 0)
	{
		// Surface an error notification in the editor.
		FNotificationInfo Info(LOCTEXT("NoRuleSet",
			"Please assign a valid Rule Set before generating."));
		Info.ExpireDuration = 4.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
		return FReply::Handled();
	}

	bIsGenerating = true;

	WFCAlgorithm = MakeShareable(new FWaveFunctionCollapse(RuleSet.Get()));
	const bool bSuccess = WFCAlgorithm->Generate(GridSize, RandomSeed, World);
	LastGenerationTime = WFCAlgorithm->GetLastGenerationTime();

	bIsGenerating = false;

	if (bSuccess)
	{
		OnGenerationCompleteDelegate.ExecuteIfBound();
	}
	else
	{
		FNotificationInfo Info(LOCTEXT("GenerationFailed",
			"Wave Function Collapse failed to find a valid solution. "
			"Try adjusting the rule set or increasing Max Backtrack Attempts."));
		Info.ExpireDuration = 5.0f;
		FSlateNotificationManager::Get().AddNotification(Info);
	}

	return FReply::Handled();
}

FReply SLevelGeneratorWidget::OnClearClicked()
{
	UWorld* World = GEditor ? GEditor->GetEditorWorldContext().World() : nullptr;
	if (World && WFCAlgorithm.IsValid())
	{
		WFCAlgorithm->ClearGeneratedActors(World);
	}
	LastGenerationTime = 0.0;
	return FReply::Handled();
}

void SLevelGeneratorWidget::RefreshStatistics()
{
	// Statistics labels use attribute bindings and refresh automatically.
}

FText SLevelGeneratorWidget::GetGenerateButtonLabel() const
{
	return bIsGenerating
		? LOCTEXT("Generating", "Generating...")
		: LOCTEXT("Generate", "Generate");
}

bool SLevelGeneratorWidget::IsGenerateEnabled() const
{
	return !bIsGenerating;
}

#undef LOCTEXT_NAMESPACE
