// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#include "WaveFunctionCollapse.h"

#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "HAL/PlatformTime.h"
#include "Math/RandomStream.h"
#include "Misc/ScopedSlowTask.h"
#include "UObject/WeakObjectPtr.h"

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

FWaveFunctionCollapse::FWaveFunctionCollapse(const UGenerationRuleSet* InRuleSet)
	: RuleSet(InRuleSet)
{
}

FWaveFunctionCollapse::~FWaveFunctionCollapse()
{
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool FWaveFunctionCollapse::Generate(const FIntVector& InGridSize, int32 Seed, UWorld* World)
{
	const double StartTime = FPlatformTime::Seconds();

	// Allow the editor to show a progress bar for large grids.
	FScopedSlowTask SlowTask(
		static_cast<float>(InGridSize.X * InGridSize.Y * InGridSize.Z),
		FText::FromString(TEXT("Generating level...")));
	SlowTask.MakeDialog(/*bShowCancelButton=*/true);

	// Seed the RNG (0 → time-based).
	RandomStream = (Seed != 0)
		? FRandomStream(Seed)
		: FRandomStream(static_cast<int32>(FPlatformTime::Cycles()));

	bool bSuccess = false;

	for (int32 Attempt = 0; Attempt < MaxRetries && !bSuccess; ++Attempt)
	{
		InitializeGrid(InGridSize);

		bSuccess = true;
		while (HasUncollapsedCells())
		{
			if (SlowTask.ShouldCancel())
			{
				LastGenerationTime = FPlatformTime::Seconds() - StartTime;
				return false;
			}

			FIntVector LowestEntropy = FindLowestEntropyCell();
			CollapseCell(LowestEntropy);

			if (!PropagateConstraints(LowestEntropy))
			{
				bSuccess = false;
				break; // Contradiction – retry with a different random path.
			}

			SlowTask.EnterProgressFrame(1.0f);
		}
	}

	if (bSuccess)
	{
		SpawnGeneratedLevel(World);
	}

	LastGenerationTime = FPlatformTime::Seconds() - StartTime;
	return bSuccess;
}

void FWaveFunctionCollapse::ClearGeneratedActors(UWorld* World)
{
	if (!World)
	{
		return;
	}

	for (TWeakObjectPtr<AActor>& WeakActor : SpawnedActors)
	{
		if (WeakActor.IsValid())
		{
			World->DestroyActor(WeakActor.Get());
		}
	}
	SpawnedActors.Empty();
}

// ---------------------------------------------------------------------------
// Grid helpers
// ---------------------------------------------------------------------------

void FWaveFunctionCollapse::InitializeGrid(const FIntVector& InGridSize)
{
	CurrentGridSize = InGridSize;

	const int32 TileCount = RuleSet.IsValid() ? RuleSet->TileModules.Num() : 0;
	const int32 CellCount = InGridSize.X * InGridSize.Y * InGridSize.Z;

	Grid.SetNum(CellCount);

	// Every cell starts as a superposition of all tile indices.
	TArray<int32> AllTiles;
	AllTiles.Reserve(TileCount);
	for (int32 i = 0; i < TileCount; ++i)
	{
		AllTiles.Add(i);
	}

	for (TArray<int32>& Cell : Grid)
	{
		Cell = AllTiles;
	}
}

int32 FWaveFunctionCollapse::CellIndex(const FIntVector& Coord) const
{
	return Coord.X + Coord.Y * CurrentGridSize.X + Coord.Z * CurrentGridSize.X * CurrentGridSize.Y;
}

bool FWaveFunctionCollapse::HasUncollapsedCells() const
{
	for (const TArray<int32>& Cell : Grid)
	{
		if (Cell.Num() > 1)
		{
			return true;
		}
	}
	return false;
}

FIntVector FWaveFunctionCollapse::FindLowestEntropyCell() const
{
	int32 LowestEntropy = TNumericLimits<int32>::Max();
	TArray<FIntVector> Candidates;

	for (int32 Z = 0; Z < CurrentGridSize.Z; ++Z)
	{
		for (int32 Y = 0; Y < CurrentGridSize.Y; ++Y)
		{
			for (int32 X = 0; X < CurrentGridSize.X; ++X)
			{
				const FIntVector Coord(X, Y, Z);
				const int32 Entropy = Grid[CellIndex(Coord)].Num();

				if (Entropy <= 1)
				{
					continue; // Already collapsed.
				}

				if (Entropy < LowestEntropy)
				{
					LowestEntropy = Entropy;
					Candidates.Reset();
					Candidates.Add(Coord);
				}
				else if (Entropy == LowestEntropy)
				{
					Candidates.Add(Coord);
				}
			}
		}
	}

	check(Candidates.Num() > 0);
	return Candidates[RandomStream.RandRange(0, Candidates.Num() - 1)];
}

void FWaveFunctionCollapse::CollapseCell(const FIntVector& Coord)
{
	TArray<int32>& Candidates = Grid[CellIndex(Coord)];

	if (Candidates.Num() <= 1)
	{
		return;
	}

	// Weighted random selection using tile weights from the rule set.
	float TotalWeight = 0.0f;
	if (RuleSet.IsValid())
	{
		for (int32 TileIndex : Candidates)
		{
			TotalWeight += RuleSet->TileModules[TileIndex].Weight;
		}
	}
	else
	{
		TotalWeight = static_cast<float>(Candidates.Num());
	}

	float Pick = RandomStream.FRandRange(0.0f, TotalWeight);
	int32 ChosenIndex = Candidates.Last(); // Fallback.

	for (int32 TileIndex : Candidates)
	{
		const float W = RuleSet.IsValid() ? RuleSet->TileModules[TileIndex].Weight : 1.0f;
		Pick -= W;
		if (Pick <= 0.0f)
		{
			ChosenIndex = TileIndex;
			break;
		}
	}

	Candidates.Reset();
	Candidates.Add(ChosenIndex);
}

bool FWaveFunctionCollapse::PropagateConstraints(const FIntVector& StartCoord)
{
	if (!RuleSet.IsValid())
	{
		return true;
	}

	// BFS propagation.
	TQueue<FIntVector> PropQueue;
	PropQueue.Enqueue(StartCoord);

	while (!PropQueue.IsEmpty())
	{
		FIntVector Current;
		PropQueue.Dequeue(Current);

		const TArray<int32>& CurrentCandidates = Grid[CellIndex(Current)];

		for (const FIntVector& Neighbour : GetNeighbours(Current))
		{
			TArray<int32>& NeighbourCandidates = Grid[CellIndex(Neighbour)];
			const int32 BeforeCount = NeighbourCandidates.Num();

			// Determine the direction from Neighbour back to Current so we can
			// look up the correct constraint axis.
			const FIntVector Delta = Current - Neighbour;
			int32 DirIndex = -1;
			for (int32 d = 0; d < 6; ++d)
			{
				if (DirectionToOffset(d) == Delta)
				{
					DirIndex = d;
					break;
				}
			}
			if (DirIndex == -1)
			{
				continue;
			}

			// The direction FROM neighbour TO current.
			const EDirection NeighbourToCurrentDir = static_cast<EDirection>(DirIndex);

			// Remove any neighbour candidates that are not allowed adjacent to
			// at least one of the current cell's remaining candidates.
			NeighbourCandidates.RemoveAll([&](int32 NeighbourTileIndex) -> bool
			{
				const FName& NeighbourName = RuleSet->TileModules[NeighbourTileIndex].ModuleName;

				for (int32 CurrentTileIndex : CurrentCandidates)
				{
					const FName& CurrentName = RuleSet->TileModules[CurrentTileIndex].ModuleName;
					if (RuleSet->IsNeighborAllowed(CurrentName, NeighbourToCurrentDir, NeighbourName))
					{
						return false; // Keep this candidate.
					}
				}
				return true; // No current candidate allows this neighbour tile.
			});

			if (NeighbourCandidates.Num() == 0)
			{
				return false; // Contradiction.
			}

			if (NeighbourCandidates.Num() < BeforeCount)
			{
				// The neighbour's domain changed; propagate from it too.
				PropQueue.Enqueue(Neighbour);
			}
		}
	}

	return true;
}

void FWaveFunctionCollapse::SpawnGeneratedLevel(UWorld* World)
{
	if (!World || !RuleSet.IsValid())
	{
		return;
	}

	const float TileSize = RuleSet->TileSize;

	for (int32 Z = 0; Z < CurrentGridSize.Z; ++Z)
	{
		for (int32 Y = 0; Y < CurrentGridSize.Y; ++Y)
		{
			for (int32 X = 0; X < CurrentGridSize.X; ++X)
			{
				const FIntVector Coord(X, Y, Z);
				const TArray<int32>& Candidates = Grid[CellIndex(Coord)];

				if (Candidates.Num() != 1)
				{
					continue; // Uncollapsed or contradiction cell – skip.
				}

				const int32 TileIndex = Candidates[0];
				const FTileModuleDefinition& Module = RuleSet->TileModules[TileIndex];

				if (!Module.Mesh)
				{
					continue; // No mesh assigned for this module.
				}

				const FVector Location(
					X * TileSize,
					Y * TileSize,
					Z * TileSize);

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride =
					ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(
					AStaticMeshActor::StaticClass(),
					FTransform(FRotator::ZeroRotator, Location),
					SpawnParams);

				if (MeshActor)
				{
					MeshActor->GetStaticMeshComponent()->SetStaticMesh(Module.Mesh);
					MeshActor->SetActorLabel(
						FString::Printf(TEXT("%s_%d_%d_%d"),
							*Module.ModuleName.ToString(), X, Y, Z));
					SpawnedActors.Add(MeshActor);
				}
			}
		}
	}
}

TArray<FIntVector> FWaveFunctionCollapse::GetNeighbours(const FIntVector& Coord) const
{
	TArray<FIntVector> Neighbours;
	Neighbours.Reserve(6);

	for (int32 d = 0; d < 6; ++d)
	{
		const FIntVector N = Coord + DirectionToOffset(d);
		if (N.X >= 0 && N.X < CurrentGridSize.X &&
			N.Y >= 0 && N.Y < CurrentGridSize.Y &&
			N.Z >= 0 && N.Z < CurrentGridSize.Z)
		{
			Neighbours.Add(N);
		}
	}

	return Neighbours;
}

FIntVector FWaveFunctionCollapse::DirectionToOffset(int32 DirectionIndex)
{
	switch (DirectionIndex)
	{
	case 0: return FIntVector( 1,  0,  0); // +X
	case 1: return FIntVector(-1,  0,  0); // -X
	case 2: return FIntVector( 0,  1,  0); // +Y
	case 3: return FIntVector( 0, -1,  0); // -Y
	case 4: return FIntVector( 0,  0,  1); // +Z
	case 5: return FIntVector( 0,  0, -1); // -Z
	default: return FIntVector::ZeroValue;
	}
}

int32 FWaveFunctionCollapse::OppositeDirection(int32 DirectionIndex)
{
	// +X(0) ↔ -X(1),  +Y(2) ↔ -Y(3),  +Z(4) ↔ -Z(5)
	return DirectionIndex ^ 1;
}
