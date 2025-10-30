// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_BuildShip.h"

#include "Commands/DMCommand.h"					// FCommandPacket
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMCommandFlagsComponent.h"	// UDMActiveCommandsComponent
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "GameSettings/DMGameMode.h"			// ADMGameMode
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip

/******************************************************************************
 * Constructor: set command flag
******************************************************************************/
UDMCommand_BuildShip::UDMCommand_BuildShip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommandFlags = ECommandFlags::MovingShip;
}

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
	if (pTargetNode->HasShip())
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship on %s, but it has a ship!"),
			*pTargetNode->GetName())

		return false;
	}

	// make the ship! note: pass in the owning players team instead of using the planet just in case we do some crazy abilities later
	pTargetPlanet->K2_SpawnShip(DMGameMode->GetDefaultShip(), pOwningPlayer->TeamComponent->GetTeam());
	return true;
}

/******************************************************************************
 * When we register, tell our target planet that a ship is incoming!
******************************************************************************/
void UDMCommand_BuildShip::CommandQueued_Implementation() /* override */
{
	pTargetNode->CommandsComponent->RegisterCommand(this);
	pTargetNode->CommandsComponent->AddCommandFlags(CommandFlags);
}

/******************************************************************************
 * When we unregister, tell our target planet no ship is coming anymore!
******************************************************************************/
void UDMCommand_BuildShip::CommandUnqueued_Implementation() /* override */
{
	pTargetNode->CommandsComponent->UnregisterCommand(this);
	pTargetNode->CommandsComponent->RemoveCommandFlags(CommandFlags);
}

/******************************************************************************
 * Make sure our planet still exists
 * returns true if command can be run successfully
******************************************************************************/
bool UDMCommand_BuildShip::Validate_Implementation() const /* override */
{
	// our core variables better be valid
	if (!Super::Validate_Implementation())
	{
		return false;
	}

	// we need to be targeting a planet and it needs to be on our team
	ADMPlanet* TargetPlanet = Cast<ADMPlanet>(pTargetNode);
	if (TargetPlanet == nullptr)
	{
		return false;
	}
	if (!TargetPlanet->TeamComponent->IsSameTeam(pOwningPlayer->TeamComponent))
	{
		return false;
	}

	return true;
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

/******************************************************************************
 * returns a string with the name of the command, what it does, and what it
 *		will operate on
******************************************************************************/
UDMCommand* UDMCommand_BuildShip::CopyCommand(const FCommandPacket& Packet) /* override */
{
	UDMCommand_BuildShip* pNewCommand = NewObject<UDMCommand_BuildShip>();
	pNewCommand->GetCopyCommandData(Packet.Data);

	return pNewCommand;
}
