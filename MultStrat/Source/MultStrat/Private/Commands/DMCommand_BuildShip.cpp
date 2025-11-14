// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_BuildShip.h"

#include "Commands/DMCommand.h"					// FCommandPacket
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMCommandFlagsComponent.h"	// UDMActiveCommandsComponent
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "GalaxyObjects/DMPlanet.h"				// ADMPlanet
#include "GameSettings/DMGameMode.h"			// UCommandsDataAsset
#include "GameSettings/DMGameState.h"			// ADMGameState
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
bool UDMCommand_BuildShip::RunCommand_Implementation() /* override */
{ 
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
	pTargetPlanet->K2_SpawnShip(ShipSpawnClass, pOwningPlayer->TeamComponent->GetTeam());
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

bool UDMCommand_BuildShip::InitializeCommand_Implementation(UDMCommandInit* InitVariables) /* override */
{
	UDMCommandInit_BuildShip* InitBuild = Cast<UDMCommandInit_BuildShip>(InitVariables);
	if (!IsValid(InitBuild))
	{
		return false;
	}
	
	// ensure our base variables work first so we can use them to get the game state
	if (!Super::InitializeCommand_Implementation(InitVariables))
	{
		return false;
	}

	if (InitBuild->ShipClass != nullptr)
	{
		ShipSpawnClass = InitBuild->ShipClass;
	}
	else
	{
		ADMGameState* pDMState = ADMGameState::Get(InitVariables->pRequestingPlayer);
		check(pDMState);
		if (pDMState->CommandsData == nullptr)
		{
			return false;
		}
		ShipSpawnClass = pDMState->CommandsData->DefaultShip;
	}

	if (ShipSpawnClass == nullptr)
	{
		return false;
	}

	return Super::InitializeCommand_Implementation(InitVariables);
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
	// do we have something to build? (were we initialized properly?)
	if (ShipSpawnClass == nullptr)
	{
		return false;
	}
	// do we have space? (if anything is wrong here, crash loudly)
	int PendingShipPower = pOwningPlayer->GetTotalShipPower() + Cast<ADMShip>(ShipSpawnClass->GetDefaultObject())->GetShipPower();
	if (pOwningPlayer->GetMaxShipPower() < PendingShipPower)
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

/******************************************************************************
 * These functions are used to fill and decode data during the CopyCommand function
******************************************************************************/
void UDMCommand_BuildShip::FillCopyCommandData(TArray<TObjectPtr<UObject>>& CommandData) /* override */
{
	Super::FillCopyCommandData(CommandData);

	CommandData.Add(ShipSpawnClass);
}

void UDMCommand_BuildShip::GetCopyCommandData(const TArray<TObjectPtr<UObject>>& CommandData) /* override */
{
	// Copy
	if (CommandData.Num() < 3)
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_BuildShip::GetCopyCommandData: Data not properly instantiated, no data will be copied"))
			return;
	}
	ShipSpawnClass = Cast<UClass>(CommandData[2]);

	// Copy parent data + call validate
	Super::GetCopyCommandData(CommandData);
}
