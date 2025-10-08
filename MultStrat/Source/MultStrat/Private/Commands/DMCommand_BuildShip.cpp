// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/DMCommand_BuildShip.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "GameSettings/DMGameMode.h"			// ADMGameMode
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip

// ----------------------------------------------------------------------------
bool UDMCommand_BuildShip::RunCommand() const /* override */
{ 
	// Get the gamemode (TODO: We have to be able to simulate on clients, who wont have access to this. Pointer to gamestate is Easy to do)
	UWorld* NodeWorld = TargetNode->GetWorld();
	AGameModeBase* GameMode = NodeWorld->GetAuthGameMode();
	ADMGameMode* DMGameMode = Cast<ADMGameMode>(GameMode);
	if (!IsValid(DMGameMode))
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship, but Gamemode inaccessible; did a client run RunCommand?"))
		
		return false;
	}

	ADMPlanet* pTargetPlanet = Cast<ADMPlanet>(TargetNode);
	if(pTargetPlanet == nullptr)
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship on invalid target %s"),
			IsValid(TargetNode) ? *TargetNode->GetName() : TEXT("NULLPTR"))
		
		return false;
	}
	if (TargetNode->K2_HasShip())
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::RunCommand: Tried to build ship on invalid target %s"),
			*TargetNode->GetName())

		return false;
	}

	// make the ship! note: pass in the owning players team instead of using the planet just in case we do some crazy abilities later
	pTargetPlanet->K2_SpawnShip(DMGameMode->GetDefaultShip(), OwningPlayer->CurrTeam);
	return true;
}

// ----------------------------------------------------------------------------
bool UDMCommand_BuildShip::Validate() const /* override */
{
	if (Cast<ADMPlanet>(TargetNode) == nullptr)
	{
		return false;
	}

	return Super::Validate();
}
// ----------------------------------------------------------------------------
FString UDMCommand_BuildShip::CommandDebug() const /* override */
{
	return FString::Printf(TEXT("Player %s attempting to build on planet %s"), 
		IsValid(OwningPlayer) ? *OwningPlayer->GetName() : *FString("INVALID PLAYER"), 
		IsValid(TargetNode)   ? *TargetNode->GetName()   : *FString("INVALID TARGET"));
}

// ----------------------------------------------------------------------------
void UDMCommand_BuildShip::CommandRegistered() /* override */
{
	TargetNode->SetIncomingShip(true, this);
}

// ----------------------------------------------------------------------------
void UDMCommand_BuildShip::CommandUnregistered() /* override */
{
	TargetNode->SetIncomingShip(false, this);
}