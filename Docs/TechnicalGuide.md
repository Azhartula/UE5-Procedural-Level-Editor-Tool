# Technical Guide – Procedural Level Generator

## Module Structure

```
ProceduralLevelGenerator/
├── ProceduralLevelGenerator.uplugin
└── Source/
    └── ProceduralLevelGenerator/
        ├── ProceduralLevelGenerator.Build.cs
        ├── Public/
        │   ├── ProceduralLevelGenerator.h   ← IModuleInterface
        │   ├── LevelGeneratorCommands.h     ← TCommands<>
        │   ├── SLevelGeneratorWidget.h      ← SCompoundWidget
        │   ├── WaveFunctionCollapse.h       ← Algorithm
        │   └── GenerationRuleSet.h          ← UDataAsset
        └── Private/
            ├── ProceduralLevelGenerator.cpp
            ├── LevelGeneratorCommands.cpp
            ├── SLevelGeneratorWidget.cpp
            ├── WaveFunctionCollapse.cpp
            └── GenerationRuleSet.cpp
```

---

## Key Classes

### `FProceduralLevelGeneratorModule`

Implements `IModuleInterface`.  Responsibilities:

- Registers the `FLevelGeneratorCommands` singleton.
- Maps commands to a `FUICommandList`.
- Extends the Level Editor toolbar and Window menu.
- Registers the nomad tab spawner (tab name: `ProceduralLevelGenerator`).

### `FLevelGeneratorCommands`

A `TCommands<>` singleton.  Defines three `FUICommandInfo` entries:
`OpenPluginWindow`, `GenerateLevel`, `ClearLevel`.

### `SLevelGeneratorWidget`

A `SCompoundWidget` divided into four vertical sections:
1. **Parameters** – grid size + seed spin-boxes.
2. **Rule Set** – `SObjectPropertyEntryBox` for picking a `UGenerationRuleSet` asset.
3. **Action Buttons** – Generate / Clear.
4. **Statistics** – last run time label, updated via a `Text_Lambda` attribute.

The widget holds a `TSharedPtr<FWaveFunctionCollapse>` that is recreated on each Generate click.

### `FWaveFunctionCollapse`

Implements the Wave Function Collapse algorithm:

| Method | Responsibility |
|---|---|
| `Generate()` | Top-level entry point; runs observe→propagate loop with retry. |
| `InitializeGrid()` | Resets every cell to the full candidate set. |
| `FindLowestEntropyCell()` | BFS-free scan; returns a random candidate among ties. |
| `CollapseCell()` | Weighted random selection; reduces cell to one tile. |
| `PropagateConstraints()` | BFS from a changed cell; removes invalid neighbours. |
| `SpawnGeneratedLevel()` | Spawns `AStaticMeshActor` per collapsed cell. |
| `ClearGeneratedActors()` | Destroys actors tracked in `SpawnedActors`. |

### `UGenerationRuleSet`

A `UDataAsset` subclass holding:
- `TArray<FTileModuleDefinition> TileModules` – the tile vocabulary.
- `float TileSize` – uniform actor scale.
- `int32 MaxBacktrackAttempts` – solver retry budget.

Helper methods `FindModuleIndexByName()` and `IsNeighborAllowed()` are exposed to Blueprint.

---

## Algorithm Deep-Dive

### Initialisation

Every grid cell starts as a *superposition* of all `N` tile indices.  The `Grid`
array stores `TArray<int32>` per cell, each initialised to `{0, 1, …, N-1}`.

### Observe

The algorithm scans the entire grid for uncollapsed cells (candidate count > 1)
and selects the one with the **fewest** remaining candidates (**minimum entropy**).
Ties are broken randomly to prevent axis-aligned artefacts.

### Collapse

The chosen cell is *collapsed*: one candidate is kept and all others discarded.
Candidate selection is **weighted** by `FTileModuleDefinition::Weight` using a
linear scan over the cumulative weight distribution.

### Propagate

A BFS queue is seeded with the just-collapsed cell.  For each cell dequeued, the
algorithm checks every ortho-adjacent neighbour and removes any tile candidate
that is not compatible with *at least one* candidate still present in the
dequeued cell.  If a neighbour's candidate set shrinks, it is added to the
queue.  A cell reaching zero candidates signals a **contradiction**.

### Backtracking

On contradiction the entire grid is re-initialised with a different random path.
The maximum number of retries is `MaxRetries` (class constant, default 10) and
can additionally be limited by `UGenerationRuleSet::MaxBacktrackAttempts`.

---

## Threading

For grids larger than ~20×20, consider moving `FWaveFunctionCollapse::Generate()`
onto a background thread via `FRunnable` / `Async()`.  The current implementation
is single-threaded but wrapped in a `FScopedSlowTask` to keep the editor
responsive via the cancel-dialog pump.

---

## Editor Integration

- **Undo/Redo** – All spawned actors are part of the editor's transaction history.
  No explicit `FScopedTransaction` is required because `UWorld::SpawnActor` is
  already transaction-aware.
- **Tab management** – The plugin registers a *nomad tab* so it can be closed and
  re-opened without destroying state.
- **Data validation** – `UGenerationRuleSet::IsDataValid()` runs in the editor
  and surfaces errors/warnings in the **Output Log** and asset validation dialog.
