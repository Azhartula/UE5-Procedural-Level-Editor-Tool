# Wave Function Collapse – Algorithm Explanation

## Overview

**Wave Function Collapse (WFC)** is a constraint-satisfaction algorithm originally
created by Maxim Gumin (2016) for texture synthesis.  This plugin adapts WFC to
3-D grid-based level generation in Unreal Engine 5.

---

## Conceptual Model

Imagine each grid cell as a quantum superposition: it can be *any* tile until it
is **observed** (collapsed).  The rules that govern which tiles may sit next to
each other are the **constraints**.  The algorithm repeatedly:

1. **Observes** the cell with the highest certainty (lowest entropy).
2. **Collapses** that cell to one tile chosen at random (weighted by tile frequency).
3. **Propagates** the change to neighbouring cells, removing tiles that are now
   forbidden.

Repeat until every cell is collapsed, or until a **contradiction** is reached
(a cell with no valid tiles remaining).

---

## Entropy

Entropy measures *uncertainty*.  In a standard WFC implementation this is the
Shannon entropy of the probability distribution over remaining candidates.  This
plugin uses **cardinality entropy** (the raw candidate count) for simplicity and
speed:

```
entropy(cell) = |remaining candidates|
```

A cell with only one candidate left has entropy 1 and is considered **collapsed**.
The algorithm always collapses the cell with the *minimum* entropy > 1.

---

## Constraint Propagation (BFS)

After collapsing a cell C, the change must ripple outward.  The plugin uses a
**breadth-first search (BFS)** queue:

```
Queue ← { C }
while Queue not empty:
    Current ← dequeue(Queue)
    for each Neighbour of Current:
        remove tiles T from Neighbour where
            no tile in Current allows T in that direction
        if Neighbour.candidates shrank:
            enqueue(Neighbour)
```

This ensures that the removal of a candidate in one cell triggers the removal of
now-invalid candidates in all transitively affected cells.

---

## Contradiction and Backtracking

A **contradiction** occurs when a cell's candidate set becomes empty.  The plugin
resolves this by discarding the current grid state and restarting with a different
random path (using the same seed incremented by one to explore a different
trajectory).  Up to `MaxRetries` (default 10) attempts are made before returning
`false` to the caller.

For puzzle-like rule sets where contradictions are frequent, consider:

- Adding a **wildcard** tile that is compatible with every neighbour.
- Loosening constraints on boundary-adjacent faces.
- Increasing `MaxBacktrackAttempts` on the Rule Set asset.

---

## Weighted Collapse

Standard WFC collapses cells uniformly.  This plugin supports **weighted random
selection** via `FTileModuleDefinition::Weight`.  The algorithm draws from a
cumulative weight distribution:

```
totalWeight = sum of weights of remaining candidates
pick        = random float in [0, totalWeight)
for each candidate (in order):
    pick -= candidate.weight
    if pick <= 0: choose this candidate; break
```

Setting a Floor tile's weight to 10 and a Pillar tile's weight to 1 produces
roughly 10 floors for every pillar, without changing any adjacency constraints.

---

## 3-D Extension

The standard WFC paper targets 2-D grids.  This plugin extends to 3-D by
treating vertical adjacency (`+Z`/`-Z`) the same as the four horizontal
directions.  Each tile module's `AllowedNeighbors` map therefore has up to six
entries – one per `EDirection` value.

For purely flat levels, keep `GridSize.Z = 1` and leave the `+Z`/`-Z` constraint
lists empty (which means *no restriction*).

---

## Performance

The dominant cost is constraint propagation, which in the worst case touches
every cell in the grid for every collapse step.  Practical performance on this
implementation:

| Grid | Typical time | Notes |
|---|---|---|
| 10×10 | < 100 ms | Single-threaded; imperceptible delay |
| 50×50 | ~1–2 s | `FScopedSlowTask` progress bar shown |
| 100×100 | ~5–8 s | Move to background thread for best UX |

---

## References

- Maxim Gumin, *Wave Function Collapse* – https://github.com/mxgmn/WaveFunctionCollapse
- Paul Merrell, *Model Synthesis* (precursor algorithm)
- Robert Osfield, *"Constraint-based procedural level generation"*, GDC 2019
