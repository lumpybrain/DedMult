// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_BlueprintLibrary.h"

#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "Commands/DMCommand.h"					// UDMCommandInit
#include "Commands/DMCommand_MoveShip.h"		// UDMCommand_MoveShip, UDMCommandInitMoveShip
#include "Commands/DMCommand_BuildShip.h"		// UDMCommand_BuildShip
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip

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

UDMCommandInitMoveShip* UDMCommand_BlueprintLibrary::MakeCommandInit_MoveShip(ADMPlayerState* pRequestingPlayer, ADMGalaxyNode* pTarget, ADMShip* pShip)
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
bool UDMCommand_BlueprintLibrary::TrialCommand_BuildShip(const ADMPlayerState* pRequestingPlayer, const ADMGalaxyNode* pPlanetToBuildOn, FString& OutFailString)
{
	// Player must be valid
	if (!IsValid(pRequestingPlayer))
	{
		OutFailString = TEXT("Invalid RequestingPlayer passed in");
		return false;
	}
	// Must be a valid node
	if (!IsValid(pPlanetToBuildOn))
	{
		OutFailString = TEXT("Invalid PlanetToBuildOn passed in");
		return false;
	}
	// Must be a planet
	if (!IsValid(Cast<ADMPlanet>(pPlanetToBuildOn)))
	{
		OutFailString = TEXT("Passed in node was not a planet; you can only build on planets!");
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

	return true;
}

/******************************************************************************
 * Build the BuildShip command
 * returns the constructed command, or a nullptr if an error occurred
******************************************************************************/
UDMCommand_BuildShip* UDMCommand_BlueprintLibrary::MakeCommand_BuildShip(ADMPlayerState* pRequestingPlayer, ADMGalaxyNode* pPlanetToBuildOn)
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

/*/////////////////////////////////////////////////////////////////////////////
*	Move Ship Command /////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Checks if the MoveShip command is possible with the passed in objects.
 * True if a move ship command is possible, false otherwise
 * OutFailString is filled with a debug reason for failure
******************************************************************************/
bool UDMCommand_BlueprintLibrary::TrialCommand_MoveShip(const ADMPlayerState* pRequestingPlayer, const ADMShip* pShip, const ADMGalaxyNode* pNodeToMoveTo, FString& OutFailString)
{
	// Player must be valid
	if (!IsValid(pRequestingPlayer))
	{
		OutFailString = TEXT("Invalid RequestingPlayer passed in");
		return false;
	}
	// Target node must be valid
	if (!IsValid(pNodeToMoveTo))
	{
		OutFailString = TEXT("Invalid NodeToMoveTo passed in");
		return false;
	}
	// Ship must be valid
	if (!IsValid(pShip))
	{
		OutFailString = TEXT("Invalid Ship passed in");
		return false;
	}
	// Player and ship must be the same team
	if (!UDMTeamComponent::ActorsAreSameTeam(pShip, pRequestingPlayer))
	{
		OutFailString = FString::Printf(TEXT("Player %s wants to move ship %s, but they are on seperate teams!"),
			*pRequestingPlayer->GetName(),
			*pShip->GetName());
		return false;
	}
	// Target node must be reachable
	if (!pShip->IsNodeReachable(pNodeToMoveTo))
	{
		OutFailString = TEXT("Target node is not reachable by the ship (Or is the node the ship is currently docked at)");
		return false;
	}

	return true;
}

/******************************************************************************
 * Build the MoveShip command
 * returns the constructed command, or a nullptr if an error occurred
******************************************************************************/
UDMCommand_MoveShip* UDMCommand_BlueprintLibrary::MakeCommand_MoveShip(ADMPlayerState* pRequestingPlayer, ADMShip* pShip, ADMGalaxyNode* pNodeToMoveTo)
{
	// Can we?
	FString FailureOutput;
	if (!TrialCommand_MoveShip(pRequestingPlayer, pShip, pNodeToMoveTo, FailureOutput))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommand_BlueprintLibrary::MakeCommand_MoveShip: %s"), *FailureOutput)
		return nullptr;
	}

	// Build it
	UDMCommand_MoveShip* NewMoveCommand = NewObject<UDMCommand_MoveShip>();
	NewMoveCommand->InitializeCommand(MakeCommandInit_MoveShip(pRequestingPlayer, pNodeToMoveTo, pShip));
	return NewMoveCommand;
}
