// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"

class UGenerationRuleSet;
class FWaveFunctionCollapse;

DECLARE_DELEGATE(FOnGenerationComplete);

/**
 * Main Slate widget for the Procedural Level Generator plugin panel.
 *
 * Provides controls for configuring the grid size, random seed, rule set asset,
 * and advanced constraint settings.  Delegates the actual generation work to
 * FWaveFunctionCollapse and reports completion through OnGenerationComplete.
 */
class SLevelGeneratorWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLevelGeneratorWidget)
		: _GridSize(FIntVector(10, 10, 1))
		, _RandomSeed(0)
		, _RuleSet(nullptr)
	{}
		/** Initial grid dimensions (X columns, Y rows, Z layers). */
		SLATE_ARGUMENT(FIntVector, GridSize)

		/** Seed used for the random number generator (0 = randomize each run). */
		SLATE_ARGUMENT(int32, RandomSeed)

		/** Rule set data asset that defines tile modules and neighbour constraints. */
		SLATE_ARGUMENT(UGenerationRuleSet*, RuleSet)

		/** Called on the game thread once generation has successfully finished. */
		SLATE_EVENT(FOnGenerationComplete, OnGenerationComplete)
	SLATE_END_ARGS()

	/** Constructs the widget and lays out all child panels. */
	void Construct(const FArguments& InArgs);

	/** Kicks off an asynchronous generation pass with the current settings. */
	FReply OnGenerateClicked();

	/** Clears all actors that were spawned during the last generation pass. */
	FReply OnClearClicked();

private:
	/** Builds the Parameters panel (grid size, seed, advanced options). */
	TSharedRef<SWidget> BuildParametersPanel();

	/** Builds the Rule Set Browser panel. */
	TSharedRef<SWidget> BuildRuleSetPanel();

	/** Builds the Statistics / performance-monitoring panel. */
	TSharedRef<SWidget> BuildStatisticsPanel();

	/** Updates the statistics labels after generation completes. */
	void RefreshStatistics();

	// -------------------------------------------------------------------------
	// State
	// -------------------------------------------------------------------------

	FIntVector GridSize;
	int32 RandomSeed;
	TWeakObjectPtr<UGenerationRuleSet> RuleSet;
	FOnGenerationComplete OnGenerationCompleteDelegate;

	TSharedPtr<FWaveFunctionCollapse> WFCAlgorithm;

	/** Wall-clock time (seconds) taken by the most recent generation pass. */
	double LastGenerationTime = 0.0;

	/** Whether a generation pass is currently running in the background. */
	bool bIsGenerating = false;

	// -------------------------------------------------------------------------
	// Attribute helpers
	// -------------------------------------------------------------------------

	/** Returns the text to show on the Generate button. */
	FText GetGenerateButtonLabel() const;

	/** Returns whether the Generate button should be interactive. */
	bool IsGenerateEnabled() const;

	/** Text bound to the X grid-size spin-box. */
	TOptional<int32> GetGridSizeX() const { return GridSize.X; }

	/** Text bound to the Y grid-size spin-box. */
	TOptional<int32> GetGridSizeY() const { return GridSize.Y; }

	/** Text bound to the seed spin-box. */
	TOptional<int32> GetSeedValue() const { return RandomSeed; }

	void OnGridSizeXChanged(int32 NewValue) { GridSize.X = FMath::Clamp(NewValue, 1, 100); }
	void OnGridSizeYChanged(int32 NewValue) { GridSize.Y = FMath::Clamp(NewValue, 1, 100); }
	void OnSeedChanged(int32 NewValue)       { RandomSeed = NewValue; }
};
