# API Reference – Procedural Level Generator

## `FProceduralLevelGeneratorModule`

**Header:** `Public/ProceduralLevelGenerator.h`

| Member | Signature | Description |
|---|---|---|
| `StartupModule` | `virtual void StartupModule() override` | Registers commands, tab spawner, and UI extensions. |
| `ShutdownModule` | `virtual void ShutdownModule() override` | Unregisters all above. |
| `OnSpawnPluginTab` | `TSharedRef<SDockTab>(const FSpawnTabArgs&)` | Creates the main plugin tab. |
| `OpenPluginWindow` | `void()` | Invokes the tab via `FGlobalTabmanager`. |

---

## `FLevelGeneratorCommands`

**Header:** `Public/LevelGeneratorCommands.h`

Inherits `TCommands<FLevelGeneratorCommands>`.

| Command | Default Shortcut | Description |
|---|---|---|
| `OpenPluginWindow` | *(none)* | Opens the plugin panel. |
| `GenerateLevel` | `Ctrl+Shift+G` | Runs generation. |
| `ClearLevel` | `Ctrl+Shift+C` | Clears spawned actors. |

---

## `SLevelGeneratorWidget`

**Header:** `Public/SLevelGeneratorWidget.h`

### Slate arguments

| Argument | Type | Default | Description |
|---|---|---|---|
| `_GridSize` | `FIntVector` | `(10, 10, 1)` | Grid dimensions. |
| `_RandomSeed` | `int32` | `0` | RNG seed (0 = random each run). |
| `_RuleSet` | `UGenerationRuleSet*` | `nullptr` | Rule set to use. |
| `_OnGenerationComplete` | `FOnGenerationComplete` | *(unbound)* | Fired after successful generation. |

### Methods

| Method | Returns | Description |
|---|---|---|
| `Construct(const FArguments&)` | `void` | Builds the widget hierarchy. |
| `OnGenerateClicked()` | `FReply` | Runs WFC and spawns actors. |
| `OnClearClicked()` | `FReply` | Removes last-generated actors. |

---

## `FWaveFunctionCollapse`

**Header:** `Public/WaveFunctionCollapse.h`

### Constructor

```cpp
explicit FWaveFunctionCollapse(const UGenerationRuleSet* InRuleSet);
```

### Public methods

| Method | Returns | Description |
|---|---|---|
| `Generate(GridSize, Seed, World)` | `bool` | Runs the WFC algorithm. Returns `true` on success. |
| `GetLastGenerationTime()` | `double` | Wall-clock time (seconds) of the last run. |
| `ClearGeneratedActors(World)` | `void` | Destroys actors from the last run. |

---

## `UGenerationRuleSet`

**Header:** `Public/GenerationRuleSet.h`

### Properties (UPROPERTY)

| Property | Type | Description |
|---|---|---|
| `TileModules` | `TArray<FTileModuleDefinition>` | All available tile modules. |
| `TileSize` | `float` | Grid cell size in Unreal units (default 100). |
| `MaxBacktrackAttempts` | `int32` | Solver retry budget (default 10). |

### Methods (UFUNCTION)

| Method | Returns | Description |
|---|---|---|
| `FindModuleIndexByName(Name)` | `int32` | Returns index or `INDEX_NONE`. |
| `IsNeighborAllowed(Source, Dir, Target)` | `bool` | Checks adjacency constraint. |

---

## `FTileModuleDefinition`

**Header:** `Public/GenerationRuleSet.h`

### Fields (UPROPERTY)

| Field | Type | Description |
|---|---|---|
| `ModuleName` | `FName` | Unique identifier. |
| `Mesh` | `TObjectPtr<UStaticMesh>` | Mesh to spawn. |
| `AllowedNeighbors` | `TMap<EDirection, TArray<FName>>` | Per-direction neighbour whitelist. |
| `Weight` | `float` | Spawn probability weight (default 1.0). |

---

## `EDirection`

**Header:** `Public/GenerationRuleSet.h`

| Enumerator | Value | Description |
|---|---|---|
| `PosX` | 0 | +X direction |
| `NegX` | 1 | -X direction |
| `PosY` | 2 | +Y direction |
| `NegY` | 3 | -Y direction |
| `PosZ` | 4 | +Z (up) |
| `NegZ` | 5 | -Z (down) |
