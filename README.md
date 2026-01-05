# Procedural Level Generator Editor Plugin

[![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5.3-blue)](https://www.unrealengine.com/)
[![C++](https://img.shields.io/badge/C++-17-orange)](https://isocpp.org/)
[![Plugin](https://img.shields.io/badge/Type-Editor%20Plugin-purple)](https://docs.unrealengine.com/5.3/en-US/plugins-in-unreal-engine/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

Professional-grade Unreal Engine 5 editor plugin for procedural level generation using Wave Function Collapse algorithm with custom Slate UI.

## 🎯 Project Overview

This plugin demonstrates mastery of Unreal Engine's editor extension capabilities, custom UI development with Slate framework, and advanced algorithm implementation. Features marketplace-ready code quality with comprehensive documentation.

## ✨ Key Features

### Custom Editor Integration
- **Slate-based UI** with dockable panels
- Custom toolbar buttons and menu integration
- Real-time parameter adjustment
- Intuitive user experience for level designers

### Procedural Generation
- **Wave Function Collapse** algorithm implementation
- Constraint-based tile placement
- Supports grids up to 100x100 cells
- Multi-threaded generation for large levels

### Asset Pipeline
- Custom Data Asset type for rule sets
- Asset browser integration
- Save/load generation configurations
- Thumbnail rendering for presets

### Real-time Preview
- Live generation preview in viewport
- Full undo/redo support
- Visual debugging tools
- Performance monitoring

## 🏗️ Architecture

```
ProceduralLevelGenerator/
├── Source/
│   ├── ProceduralLevelGenerator/
│   │   ├── Private/
│   │   │   ├── ProceduralLevelGenerator.cpp
│   │   │   ├── LevelGeneratorCommands.cpp
│   │   │   ├── SLevelGeneratorWidget.cpp
│   │   │   ├── WaveFunctionCollapse.cpp
│   │   │   └── GenerationRuleSet.cpp
│   │   └── Public/
│   │       └── [Headers]
│   └── ProceduralLevelGeneratorEditor/
│       └── [Editor modules]
├── Content/
│   ├── ExampleRuleSets/
│   └── Presets/
└── Resources/
    ├── Icons/
    └── UI/
```

## 🛠️ Technical Highlights

### Editor Tools Development
- Custom Slate widgets and panels
- Editor mode extensions
- Command system integration
- Asset factory implementation

### Algorithm Implementation
- Wave Function Collapse with backtracking
- Constraint propagation system
- Entropy calculation and optimization
- Multi-threaded execution

### Unreal Engine Systems
- Reflection system (UPROPERTY/UFUNCTION)
- Custom asset types
- Editor transactions (undo/redo)
- Viewport rendering integration

## 🔧 Technologies Used

- **C++** - Modern C++ with STL
- **Slate UI Framework** - Custom editor interface
- **Unreal Editor API** - Plugin integration
- **Wave Function Collapse** - Procedural generation
- **Asset Management** - Custom asset pipeline

## 📊 Performance

| Grid Size | Generation Time | Threading |
|-----------|----------------|-----------|
| 10x10 | < 100ms | Single |
| 50x50 | < 2s | Multi |
| 100x100 | < 8s | Multi |

## 🚀 Installation

### From Source
1. Clone repository to your project's `Plugins` folder
```bash
cd YourProject/Plugins
git clone https://github.com/Azhartula/UE5-Procedural-Level-Editor-Tool.git
```

2. Regenerate project files
```bash
Right-click YourProject.uproject → Generate Visual Studio project files
```

3. Build the project
```bash
Open solution in Visual Studio → Build
```

4. Launch Unreal Editor
- Plugin will appear in Edit → Plugins
- Enable and restart editor

### Pre-built Package
Download the latest release from [Releases](https://github.com/Azhartula/UE5-Procedural-Level-Editor-Tool/releases)

## 🎮 Usage

### Opening the Tool
1. In Unreal Editor, go to **Window → Procedural Level Generator**
2. Dock the panel anywhere in the editor
3. Select or create a rule set asset

### Creating a Rule Set
1. Right-click in Content Browser → **Miscellaneous → Data Asset**
2. Select **GenerationRuleSet** as the class
3. Configure modules and constraints
4. Save the asset

### Generating Levels
1. Open the plugin window
2. Select your rule set
3. Adjust grid size and parameters
4. Click **Generate**
5. Use Undo/Redo as needed

### Example Rule Set
```cpp
// In Blueprint or C++
UGenerationRuleSet* RuleSet = NewObject<UGenerationRuleSet>();

FTileModuleDefinition Module;
Module.ModuleName = "Floor";
Module.Mesh = FloorMesh;
Module.AllowedNeighbors[EDirection::PosX].Add("Wall");

RuleSet->TileModules.Add(Module);
```

## 🎨 Slate UI Components

### Main Widget
```cpp
SNew(SLevelGeneratorWidget)
    .GridSize(FIntVector(10, 10, 1))
    .RuleSet(MyRuleSet)
    .OnGenerationComplete(this, &MyClass::HandleComplete)
```

### Custom Panels
- **Parameters Panel** - Grid size, seed, constraints
- **Rule Set Browser** - Visual rule set selection
- **Preview Viewport** - Real-time generation view
- **Statistics Panel** - Performance metrics

## 📚 Code Examples

### Custom Editor Command
```cpp
void FProceduralLevelGeneratorModule::RegisterCommands()
{
    FLevelGeneratorCommands::Register();
    
    PluginCommands->MapAction(
        FLevelGeneratorCommands::Get().OpenPluginWindow,
        FExecuteAction::CreateRaw(this, &FProceduralLevelGeneratorModule::OnSpawnPluginTab)
    );
}
```

### Wave Function Collapse
```cpp
bool FWaveFunctionCollapse::Generate(const FIntVector& GridSize)
{
    InitializeGrid(GridSize);
    
    while (HasUncollapsedCells())
    {
        FIntVector LowestEntropy = FindLowestEntropyCell();
        CollapseCell(LowestEntropy);
        
        if (!PropagateConstraints(LowestEntropy))
            return false; // Contradiction
    }
    
    SpawnGeneratedLevel();
    return true;
}
```

## 📖 Documentation

Comprehensive documentation available:
- [User Guide](Docs/UserGuide.md) - How to use the plugin
- [Technical Guide](Docs/TechnicalGuide.md) - Architecture details
- [API Reference](Docs/API.md) - C++ API documentation
- [Algorithm Explanation](Docs/WaveFunctionCollapse.md) - WFC details

## 🎓 What This Demonstrates

### Editor Tools Development
- ✅ Custom Slate UI creation
- ✅ Editor extension and integration
- ✅ Asset pipeline development
- ✅ Transaction system (undo/redo)

### Software Engineering
- ✅ Plugin architecture design
- ✅ Modular, maintainable code
- ✅ Comprehensive documentation
- ✅ Professional code quality

### Algorithm Implementation
- ✅ Wave Function Collapse
- ✅ Constraint solving
- ✅ Performance optimization
- ✅ Multi-threading

## 🔍 Code Quality

- ✅ Unreal Engine coding standards
- ✅ Doxygen-style documentation
- ✅ Modular architecture (SOLID principles)
- ✅ Extensive error handling
- ✅ Performance profiling

## 🤝 Contributing

This is a portfolio project. Feedback and suggestions welcome via Issues!

## 📄 License

MIT License - see [LICENSE](LICENSE) file

## 👤 Author

**Azhar Iqbal**
- GitHub: [@Azhartula](https://github.com/Azhartula)
- LinkedIn: [Azhar Iqbal](https://linkedin.com/in/azhariqbal)
- Portfolio: [yourportfolio.com](https://yourportfolio.com)

## 🌟 Acknowledgments

- Epic Games for Unreal Engine
- Wave Function Collapse algorithm by Maxim Gumin
- Unreal Slackers community for feedback

## 📸 Screenshots

![Plugin UI](Docs/Images/plugin_ui.png)
![Generated Level](Docs/Images/generated_level.png)
![Rule Set Editor](Docs/Images/ruleset_editor.png)

---

⭐ Star this repo if you find it useful for learning Unreal Engine plugin development!
