// Copyright (c) 2024 Azhar Iqbal. All Rights Reserved.

#include "GenerationRuleSet.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

int32 UGenerationRuleSet::FindModuleIndexByName(FName ModuleName) const
{
	for (int32 i = 0; i < TileModules.Num(); ++i)
	{
		if (TileModules[i].ModuleName == ModuleName)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

bool UGenerationRuleSet::IsNeighborAllowed(
	FName SourceModuleName,
	EDirection Direction,
	FName InModuleName) const
{
	const int32 SourceIndex = FindModuleIndexByName(SourceModuleName);
	if (SourceIndex == INDEX_NONE)
	{
		// Unknown source module – allow everything to avoid blocking generation.
		return true;
	}

	const FTileModuleDefinition& Source = TileModules[SourceIndex];
	const TArray<FName>* AllowedList = Source.AllowedNeighbors.Find(Direction);

	if (!AllowedList || AllowedList->Num() == 0)
	{
		// No restriction defined for this direction – allow everything.
		return true;
	}

	return AllowedList->Contains(InModuleName);
}

#if WITH_EDITOR
EDataValidationResult UGenerationRuleSet::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (TileModules.Num() == 0)
	{
		Context.AddError(FText::FromString(
			TEXT("GenerationRuleSet has no tile modules defined. "
			     "Add at least one tile module before using this rule set.")));
		Result = EDataValidationResult::Invalid;
	}

	for (int32 i = 0; i < TileModules.Num(); ++i)
	{
		const FTileModuleDefinition& Module = TileModules[i];

		if (Module.ModuleName.IsNone())
		{
			Context.AddError(FText::Format(
				FText::FromString(TEXT("Tile module at index {0} has an empty ModuleName.")),
				FText::AsNumber(i)));
			Result = EDataValidationResult::Invalid;
		}

		if (!Module.Mesh)
		{
			Context.AddWarning(FText::Format(
				FText::FromString(
					TEXT("Tile module '{0}' (index {1}) has no mesh assigned. "
					     "It will be skipped during generation.")),
				FText::FromName(Module.ModuleName),
				FText::AsNumber(i)));
		}

		if (Module.Weight < 0.0f)
		{
			Context.AddError(FText::Format(
				FText::FromString(TEXT("Tile module '{0}' has a negative Weight.")),
				FText::FromName(Module.ModuleName)));
			Result = EDataValidationResult::Invalid;
		}
	}

	return Result;
}
#endif
