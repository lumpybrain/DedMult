// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "DMCommand_BlueprintLibrary.generated.h"

class ADMPlanet;
class ADMPlayerState;
class UDMCommand_BuildShip;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	static bool ValidateCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMPlanet* PlanetToBuildOn, FString& OutFailString);
	UFUNCTION(BlueprintCallable)
	static UDMCommand_BuildShip* MakeCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMPlanet* PlanetToBuild);
	
};
