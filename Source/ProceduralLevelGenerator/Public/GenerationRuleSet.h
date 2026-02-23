// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/StaticMesh.h"
#include "GenerationRuleSet.generated.h"

/**
 * The six cardinal directions used to describe tile adjacency.
 */
UENUM(BlueprintType)
enum class EDirection : uint8
{
	PosX UMETA(DisplayName = "+X"),
	NegX UMETA(DisplayName = "-X"),
	PosY UMETA(DisplayName = "+Y"),
	NegY UMETA(DisplayName = "-Y"),
	PosZ UMETA(DisplayName = "+Z"),
	NegZ UMETA(DisplayName = "-Z"),
};

/**
 * Describes a single tile module: the mesh to place and which neighbouring
 * tiles are permitted on each of its six faces.
 */
USTRUCT(BlueprintType)
struct PROCEDURALLEVELGENERATOR_API FTileModuleDefinition
{
	GENERATED_BODY()

	/** Human-readable identifier used to reference this module in constraints. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Module")
	FName ModuleName;

	/** Static mesh rendered at grid cells that collapse to this module. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Module")
	TObjectPtr<UStaticMesh> Mesh;

	/**
	 * For each direction, the set of module names that are permitted to appear
	 * adjacent to this module on that face.  An empty set means no restriction.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Module")
	TMap<EDirection, TArray<FName>> AllowedNeighbors;

	/**
	 * Relative weight used during WFC collapse.  Higher values make this module
	 * more likely to be chosen when the cell entropy is resolved.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tile Module",
		meta = (ClampMin = "0.0", ClampMax = "100.0"))
	float Weight = 1.0f;
};

/**
 * Data asset that holds the complete set of tile modules and global generation
 * settings for one procedural level configuration.
 *
 * Create an instance via the Content Browser:
 *   Right-click → Miscellaneous → Data Asset → GenerationRuleSet
 */
UCLASS(BlueprintType)
class PROCEDURALLEVELGENERATOR_API UGenerationRuleSet : public UDataAsset
{
	GENERATED_BODY()

public:
	/** All tile modules available for placement during generation. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule Set")
	TArray<FTileModuleDefinition> TileModules;

	/**
	 * Uniform scale applied to every spawned tile actor so that mesh dimensions
	 * match the desired grid cell size.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule Set",
		meta = (ClampMin = "1.0"))
	float TileSize = 100.0f;

	/**
	 * Maximum number of times the WFC solver may backtrack before reporting
	 * failure for a single generation attempt.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rule Set",
		meta = (ClampMin = "1", ClampMax = "100"))
	int32 MaxBacktrackAttempts = 10;

	/**
	 * Returns the index of the module with the given name, or INDEX_NONE if no
	 * such module exists in TileModules.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rule Set")
	int32 FindModuleIndexByName(FName ModuleName) const;

	/**
	 * Returns true when InModuleName is permitted to sit next to
	 * SourceModuleName in Direction.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rule Set")
	bool IsNeighborAllowed(FName SourceModuleName, EDirection Direction, FName InModuleName) const;

#if WITH_EDITOR
	/** Validates the rule set data when it is saved in the editor. */
	virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
