// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenerationRuleSet.h"

/**
 * Implements the Wave Function Collapse (WFC) procedural generation algorithm.
 *
 * The algorithm works in three repeating steps:
 *  1. **Observe** – pick the uncollapsed cell with the lowest entropy (fewest
 *     remaining candidates) and collapse it to a single, randomly-chosen tile.
 *  2. **Propagate** – remove tiles from neighbouring cells that are no longer
 *     compatible with the collapsed cell's constraints, using a BFS queue.
 *  3. **Repeat** – continue until every cell is collapsed or a contradiction is
 *     detected.  On contradiction the algorithm backtracks and retries.
 *
 * Generation can optionally run on a background thread via FRunnable so that
 * large grids (up to 100×100) do not block the editor UI.
 */
class PROCEDURALLEVELGENERATOR_API FWaveFunctionCollapse
{
public:
	/**
	 * Constructs a solver for the given rule set.
	 * @param InRuleSet - Data asset describing all tile modules and their
	 *                    allowed neighbour relationships.
	 */
	explicit FWaveFunctionCollapse(const UGenerationRuleSet* InRuleSet);

	~FWaveFunctionCollapse();

	/**
	 * Runs the WFC algorithm synchronously and spawns actors in the current
	 * world when generation succeeds.
	 *
	 * @param GridSize  - Dimensions of the grid (X, Y, Z).  Z is typically 1
	 *                    for flat levels.
	 * @param Seed      - Random seed.  Pass 0 to use a time-based seed.
	 * @param World     - The UWorld into which generated actors are spawned.
	 * @return          - true on success; false if a contradiction could not be
	 *                    resolved after MaxRetries attempts.
	 */
	bool Generate(const FIntVector& GridSize, int32 Seed, UWorld* World);

	/** Returns the wall-clock time (seconds) taken by the last Generate() call. */
	double GetLastGenerationTime() const { return LastGenerationTime; }

	/**
	 * Removes all actors that were spawned during the last successful Generate()
	 * call.  Safe to call even if no level has been generated yet.
	 */
	void ClearGeneratedActors(UWorld* World);

private:
	// -------------------------------------------------------------------------
	// Grid helpers
	// -------------------------------------------------------------------------

	/** Re-initialises the grid so every cell holds all possible tile indices. */
	void InitializeGrid(const FIntVector& InGridSize);

	/**
	 * Returns the linear array index for a 3-D grid coordinate.
	 * Asserts in Debug builds if the coordinate is out of range.
	 */
	int32 CellIndex(const FIntVector& Coord) const;

	/** Returns true when at least one cell still has more than one candidate. */
	bool HasUncollapsedCells() const;

	/**
	 * Finds the uncollapsed cell with the fewest remaining candidates (lowest
	 * entropy).  Ties are broken randomly to avoid predictable patterns.
	 */
	FIntVector FindLowestEntropyCell() const;

	/**
	 * Collapses the cell at Coord to a single randomly-selected candidate and
	 * records the choice in the undo stack.
	 */
	void CollapseCell(const FIntVector& Coord);

	/**
	 * Propagates constraint changes outward from StartCoord using BFS until
	 * the wave-front is empty or a contradiction is detected.
	 * @return false if a cell's candidate set becomes empty (contradiction).
	 */
	bool PropagateConstraints(const FIntVector& StartCoord);

	/**
	 * Spawns Static Mesh Actors into World for every collapsed cell and records
	 * the spawned actors so they can be removed later.
	 */
	void SpawnGeneratedLevel(UWorld* World);

	/** Returns the 6 orthogonal neighbours of Coord that lie inside the grid. */
	TArray<FIntVector> GetNeighbours(const FIntVector& Coord) const;

	/** Converts a direction index (0-5 for ±X, ±Y, ±Z) to a grid offset. */
	static FIntVector DirectionToOffset(int32 DirectionIndex);

	/** Converts a direction index to its opposite (e.g. +X → -X). */
	static int32 OppositeDirection(int32 DirectionIndex);

	// -------------------------------------------------------------------------
	// State
	// -------------------------------------------------------------------------

	/** Grid dimensions set by the most recent call to InitializeGrid(). */
	FIntVector CurrentGridSize;

	/**
	 * Per-cell candidate sets.  Each TArray<int32> holds the indices (into
	 * RuleSet->TileModules) that are still compatible for that cell.
	 */
	TArray<TArray<int32>> Grid;

	/** Weak reference to the rule set supplied at construction time. */
	TWeakObjectPtr<const UGenerationRuleSet> RuleSet;

	/** Random stream seeded by the value passed to Generate(). */
	FRandomStream RandomStream;

	/** Actors spawned during the last successful generation pass. */
	TArray<TWeakObjectPtr<AActor>> SpawnedActors;

	/** Wall-clock duration (seconds) of the last Generate() call. */
	double LastGenerationTime = 0.0;

	/** Maximum number of backtrack retries before reporting failure. */
	static constexpr int32 MaxRetries = 10;
};
