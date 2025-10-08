// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/DMCommand_BlueprintLibrary.h"

#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "Commands/DMCommand_BuildShip.h"		// UDMCommand_BuildShip
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Player/DMPlayerState.h"				// ADMPlayerState


bool UDMCommand_BlueprintLibrary::ValidateCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMPlanet* PlanetToBuildOn, FString& OutFailString)
{
	if (!IsValid(RequestingPlayer))
	{
		OutFailString = "Invalid RequestingPlayer passed in";
		return false;
	}
	if (!IsValid(PlanetToBuildOn))
	{
		OutFailString = "Invalid PlanetToBuildOn passed in";
		return false;
	}
	if (PlanetToBuildOn->OwningTeam != RequestingPlayer->CurrTeam)
	{
		OutFailString = FString::Printf(TEXT("Player %s wants to build on planet %s, but they are on seperate teams!"), *RequestingPlayer->GetName(), *PlanetToBuildOn->GetName());
		return false;
	}
	if (PlanetToBuildOn->K2_HasShip())
	{
		OutFailString = FString::Printf(TEXT("Player %s wants to build on planet %s, but that planet already has a ship!"), *RequestingPlayer->GetName(), *PlanetToBuildOn->GetName());
		return false;
	}

	return true;
}

UDMCommand_BuildShip* UDMCommand_BlueprintLibrary::MakeCommand_BuildShip(ADMPlayerState* RequestingPlayer, ADMPlanet* PlanetToBuildOn)
{
	FString FailureOutput;
	if (!ValidateCommand_BuildShip(RequestingPlayer, PlanetToBuildOn, FailureOutput))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommand_BlueprintLibrary::MakeCommand_BuildShip: %s"), *FailureOutput)
		return nullptr;
	}

	UDMCommand_BuildShip* NewBuildCommand = NewObject<UDMCommand_BuildShip>();
	NewBuildCommand->InitializeCommand(RequestingPlayer, PlanetToBuildOn);
	return NewBuildCommand;
}

