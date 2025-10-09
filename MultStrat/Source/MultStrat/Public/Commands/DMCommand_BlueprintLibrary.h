// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DMCommand_BlueprintLibrary.generated.h"

class ADMPlanet;
class ADMPlayerState;
class UDMCommand_BuildShip;

/**
 * Blueprint library to test if commands are possible (for UI) and to build command objects (for blueprints)
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	//~=============================================================================
	// Build Ship Command
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Checks if the BuildShip command is possible with the passed in objects."))
	static bool TrialCommand_BuildShip(const ADMPlayerState* RequestingPlayer, const ADMPlanet* PlanetToBuildOn, FString& OutFailString);
	UFUNCTION(BlueprintCallable)
	static UDMCommand_BuildShip* MakeCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMPlanet* PlanetToBuild);
	
};
