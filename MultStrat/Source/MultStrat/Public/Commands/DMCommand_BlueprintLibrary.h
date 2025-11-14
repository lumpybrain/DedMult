// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DMCommand_BlueprintLibrary.generated.h"

class UDMCommandInit;
class UDMCommandInit_MoveShip;
class UDMCommandInit_BuildShip;
class ADMPlanet;
class ADMPlayerState;
class UDMCommand_BuildShip;
class UDMCommand_MoveShip;

/**
 * Blueprint library to test if commands are possible (for UI) and to build command objects (for blueprints)
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	//~=============================================================================
	// Building Command Initializers

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UDMCommandInit* MakeCommandInit(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UDMCommandInit_MoveShip* MakeCommandInit_MoveShip(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target, ADMShip* Ship);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UDMCommandInit_BuildShip* MakeCommandInit_BuildShip(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target, TSubclassOf<ADMShip> ShipOverrideType);
	
	//~=============================================================================
	// Build Ship Command

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Checks if the BuildShip command is possible with the passed in objects."))
	static bool TrialCommand_BuildShip(const ADMPlayerState* RequestingPlayer, const ADMGalaxyNode* PlanetToBuildOn, TSubclassOf<ADMShip> ShipToBuild, FString& OutFailString);

	UFUNCTION(BlueprintCallable)
	static UDMCommand_BuildShip* MakeCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* PlanetToBuild, TSubclassOf<ADMShip> ShipToBuild = nullptr);

	//~=============================================================================
	// Move Ship Command

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Checks if the MoveShip command is possible with the passed in objects."))
	static bool TrialCommand_MoveShip(const ADMPlayerState* RequestingPlayer, const ADMShip* Ship, const ADMGalaxyNode* NodeToMoveTo, FString& OutFailString);

	UFUNCTION(BlueprintCallable)
	static UDMCommand_MoveShip* MakeCommand_MoveShip(ADMPlayerState* RequestingPlayer, ADMShip* Ship, ADMGalaxyNode* NodeToMoveTo);

	//~=============================================================================
	// Support Command

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Checks if the MoveShip command is possible with the passed in objects."))
	static bool TrialCommand_Support(const ADMPlayerState* RequestingPlayer, const ADMShip* Ship, const ADMGalaxyNode* pNodeToSupport, FString& OutFailString);

	UFUNCTION(BlueprintCallable)
	static UDMCommand_MoveShip* MakeCommand_Support(ADMPlayerState* RequestingPlayer, ADMShip* Ship, ADMGalaxyNode* NodeToSupport);
	
};
