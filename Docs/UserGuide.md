# User Guide – Procedural Level Generator

## Getting Started

### Installation

1. Copy the `ProceduralLevelGenerator` folder into your project's `Plugins` directory.
2. Right-click your `.uproject` file and select **Generate Visual Studio project files**.
3. Build the project in Visual Studio (Development Editor configuration).
4. Launch Unreal Editor; the plugin will be listed under **Edit → Plugins → Editor**.
5. Enable the plugin and restart the editor when prompted.

---

## Opening the Tool

Navigate to **Window → Procedural Level Generator** in the main menu bar.  The panel is a *nomad tab* – you can dock it anywhere in the editor layout.

You can also click the **Procedural Level Generator** button that appears in the main toolbar (next to the Settings group).

---

## Creating a Rule Set

A *Rule Set* is a Data Asset that defines the tile vocabulary and adjacency rules used during generation.

1. In the **Content Browser** right-click and choose **Miscellaneous → Data Asset**.
2. Select **GenerationRuleSet** as the parent class.
3. Name and save the asset (e.g. `DA_DungeonRuleSet`).
4. Open the asset and add **Tile Modules**:
   | Property | Description |
   |---|---|
   | Module Name | Unique identifier referenced by neighbour constraints. |
   | Mesh | Static mesh placed at grid cells that collapse to this module. |
   | Allowed Neighbors | Per-direction lists of module names that may appear adjacent. |
   | Weight | Relative spawn probability (higher = more frequent). |

5. Save the asset with **Ctrl+S**.  The editor will validate the data and warn you of common issues (missing mesh, empty name, etc.).

---

## Generating a Level

1. Open the **Procedural Level Generator** panel.
2. Set the **Grid Width** and **Grid Height** (1–100 each).
3. Optionally set a **Random Seed** (leave at `0` for a different result each run).
4. Click the **Rule Set** picker and select the asset you created.
5. Click **Generate**.
   - A progress dialog appears for large grids.
   - The **Statistics** row at the bottom updates with the wall-clock time.
6. Inspect the result in the viewport.

### Undoing Generation

All spawned actors are normal UE5 world actors.  Press **Ctrl+Z** in the editor to undo the entire spawn batch, or click **Clear** in the plugin panel to remove only the last batch.

---

## Keyboard Shortcuts

| Action | Shortcut |
|---|---|
| Open plugin panel | *(toolbar button)* |
| Generate level | **Ctrl+Shift+G** |
| Clear generated level | **Ctrl+Shift+C** |

---

## Tips

- **Grid size vs. performance** – see the performance table in the README.  For grids above 50×50, ensure multi-threading is active (it is on by default).
- **Contradiction errors** – if generation fails with "no valid solution", loosen your neighbour constraints or increase **Max Backtrack Attempts** on the Rule Set asset.
- **Weights** – assigning a higher `Weight` to a "Floor" module and a lower weight to a "Treasure Room" module produces rare rooms naturally, without changing the constraint graph.
