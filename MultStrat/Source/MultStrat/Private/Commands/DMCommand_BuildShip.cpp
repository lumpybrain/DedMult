// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/DMCommand_BuildShip.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "GameSettings/DMGameMode.h"			// ADMGameMode
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip

/*/////////////////////////////////////////////////////////////////////////////
*	UDMCommand Interface //////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Build the ship on the planet!
 * returns true if command executes successfully
******************************************************************************/
bool UDMCommand_BuildShip::RunCommand_Implementation() const /* override */
{ 
	// Get the gamemode 
	// DMTODO: We have to be able to simulate on clients, who wont have access to this. Pointer to gamestate.
	UWorld* NodeWorld = pTargetNode->GetWorld();
	AGameModeBase* GameMode = NodeWorld->GetAuthGameMode();
	ADMGameMode* DMGameMode = Cast<ADMGameMode>(GameMode);
	if (!IsValid(DMGameMode))
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship, but Gamemode inaccessible; did a client run RunCommand?"))
		
		return false;
	}

	// Validate planet
	ADMPlanet* pTargetPlanet = Cast<ADMPlanet>(pTargetNode);
	if(pTargetPlanet == nullptr)
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship on invalid target %s"),
			IsValid(pTargetNode) ? *pTargetNode->GetName() : TEXT("NULLPTR"))
		
		return false;
	}
	if (pTargetNode->K2_HasShip())
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship on invalid target %s"),
			*pTargetNode->GetName())

		return false;
	}

	// make the ship! note: pass in the owning players team instead of using the planet just in case we do some crazy abilities later
	pTargetPlanet->K2_SpawnShip(DMGameMode->GetDefaultShip(), pOwningPlayer->CurrTeam);
	return true;
}

/******************************************************************************
 * When we register, tell our target planet that a ship is incoming!
******************************************************************************/
void UDMCommand_BuildShip::CommandRegistered_Implementation() /* override */
{
	pTargetNode->SetIncomingShip(true, this);
}

/******************************************************************************
 * When we unregister, tell our target planet no ship is coming anymore!
******************************************************************************/
void UDMCommand_BuildShip::CommandUnregistered_Implementation() /* override */
{
	pTargetNode->SetIncomingShip(false, this);
}

/******************************************************************************
 * Make sure our planet still exists
 * returns true if command can be run successfully
******************************************************************************/
bool UDMCommand_BuildShip::Validate_Implementation() const /* override */
{
	if (Cast<ADMPlanet>(pTargetNode) == nullptr)
	{
		return false;
	}

	return Super::Validate_Implementation();
}

/******************************************************************************
 * returns a string with the name of the command, what it does, and what it 
 *		will operate on
******************************************************************************/
FString UDMCommand_BuildShip::CommandDebug_Implementation() const /* override */
{
	return FString::Printf(TEXT("Player %s attempting to build on planet %s"), 
		IsValid(pOwningPlayer) ? *pOwningPlayer->GetName() : *FString("INVALID PLAYER"), 
		IsValid(pTargetNode)   ? *pTargetNode->GetName()   : *FString("INVALID TARGET"));
}
