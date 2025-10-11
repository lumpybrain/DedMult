// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_BlueprintLibrary.h"

#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "Commands/DMCommand.h"					// UDMCommandInit
#include "Commands/DMCommand_MoveShip.h"		// UDMCommandInitMoveShip
#include "Commands/DMCommand_BuildShip.h"		// UDMCommand_BuildShip
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "Player/DMPlayerState.h"				// ADMPlayerState


/*/////////////////////////////////////////////////////////////////////////////
*	Building Command Initializers /////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////


UDMCommandInit* UDMCommand_BlueprintLibrary::MakeCommandInit(ADMPlayerState* pRequestingPlayer, ADMGalaxyNode* pTarget)
{
	UDMCommandInit* NewInit = NewObject<UDMCommandInit>();
	NewInit->pRequestingPlayer = pRequestingPlayer;
	NewInit->pTarget = pTarget;

	return NewInit;
}

UDMCommandInitMoveShip* UDMCommand_BlueprintLibrary::MakeCommandInitMoveShip(ADMPlayerState* pRequestingPlayer, ADMGalaxyNode* pTarget, ADMShip* pShip)
{
	UDMCommandInitMoveShip* NewInit = NewObject<UDMCommandInitMoveShip>();
	NewInit->pRequestingPlayer = pRequestingPlayer;
	NewInit->pTarget = pTarget;
	NewInit->pShip = pShip;

	return NewInit;
}


/*/////////////////////////////////////////////////////////////////////////////
*	Build Ship Command ////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Is a buildShip command valid for the given inputs?
 * true if a build ship command is possible, false otherwise
 * OutFailString is filled with a debug reason for failure
******************************************************************************/
bool UDMCommand_BlueprintLibrary::TrialCommand_BuildShip(const ADMPlayerState* pRequestingPlayer, const ADMPlanet* pPlanetToBuildOn, FString& OutFailString)
{
	// Player must be valid
	if (!IsValid(pRequestingPlayer))
	{
		OutFailString = TEXT("Invalid RequestingPlayer passed in");
		return false;
	}
	// Planet must be valid
	if (!IsValid(pPlanetToBuildOn))
	{
		OutFailString = TEXT("Invalid PlanetToBuildOn passed in");
		return false;
	}
	// Player and Planet must be the same team
	if (!UDMTeamComponent::ActorsAreSameTeam(pPlanetToBuildOn, pRequestingPlayer))
	{
		OutFailString = FString::Printf(TEXT("Player %s wants to build on planet %s, but they are on seperate teams!"), 
				*pRequestingPlayer->GetName(), 
				*pPlanetToBuildOn->GetName());
		return false;
	}
	// No doubling on ships!
	if (pPlanetToBuildOn->HasShip())
	{
		OutFailString = FString::Printf(TEXT("Player %s wants to build on planet %s, but that planet already has a ship!"),
				*pRequestingPlayer->GetName(),
				*pPlanetToBuildOn->GetName());
		return false;
	}

	return true;
}

/******************************************************************************
 * Build the BuildShip command
 * returns the constructed command, or a nullptr if an error occurred
******************************************************************************/
UDMCommand_BuildShip* UDMCommand_BlueprintLibrary::MakeCommand_BuildShip(ADMPlayerState* pRequestingPlayer, ADMPlanet* pPlanetToBuildOn)
{
	// Can we?
	FString FailureOutput;
	if (!TrialCommand_BuildShip(pRequestingPlayer, pPlanetToBuildOn, FailureOutput))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommand_BlueprintLibrary::MakeCommand_BuildShip: %s"), *FailureOutput)
		return nullptr;
	}

	// Build it
	UDMCommand_BuildShip* NewBuildCommand = NewObject<UDMCommand_BuildShip>();
	NewBuildCommand->InitializeCommand(MakeCommandInit(pRequestingPlayer, pPlanetToBuildOn));
	return NewBuildCommand;
}

