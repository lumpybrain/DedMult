// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_MoveShip.h"

#include "Commands/DMCommand.h"					// FCommandPacket
#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMCommandFlagsComponent.h"	// UDMActiveCommandsComponent
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip

/******************************************************************************
 * Constructor: Set command flag
******************************************************************************/
UDMCommand_MoveShip::UDMCommand_MoveShip(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CommandFlags = ECommandFlags::MovingShip;
}

/*/////////////////////////////////////////////////////////////////////////////
*	UDMCommand Interface //////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * If the node is there, move to it
 * returns true if command executes successfully
******************************************************************************/
bool UDMCommand_MoveShip::RunCommand_Implementation() const /* override */
{
	// DMTODO: Hey, what happens if two ships are trying to move from planet A to B, and from B to A?
	if (!IsValid(pTargetNode) || !IsValid(pShip))
	{
		return false;
	}

	if (ADMGalaxyNode* pCurrentNode = pShip->GetCurrentNode())
	{
		if (!pCurrentNode->ReserveTraversalTo(pTargetNode, pShip))
		{
			return false;
		}
	}


	pShip->CommandsComponent->AddCommandFlags(ECommandFlags::MovingShip);

	pTargetNode->AddPendingShip(pShip, false, this);

	return true;
}

/******************************************************************************
 * When we register, tell our target planet that a ship is incoming!
******************************************************************************/
void UDMCommand_MoveShip::CommandQueued_Implementation() /* override */
{
	check(pTargetNode)
	// tell the target we're coming, and tell the node we're on we're moving
	pOriginalNode = pShip->GetCurrentNode();
	pTargetNode->CommandsComponent->AddCommandFlags(CommandFlags);
	pOriginalNode->CommandsComponent->RegisterCommand(this);
}

/******************************************************************************
 * When we unregister, tell our target planet no ship is coming anymore!
******************************************************************************/
void UDMCommand_MoveShip::CommandUnqueued_Implementation() /* override */
{
	check(pOriginalNode)
	check(pTargetNode)
	// tell the target we're no longer coming, and tell the node we were on
	// that we're done moving
	pTargetNode->CommandsComponent->RemoveCommandFlags(CommandFlags);
	pOriginalNode->CommandsComponent->UnregisterCommand(this);
	pOriginalNode = nullptr;
}

/******************************************************************************
 * Initialize variables. InitVariables must be of class UDMCommandInitMoveShip.
 * returns whether init was successful. Init can be unsuccessful if
 *		the input is not properly initialized (i.e passing in nullptrs)
******************************************************************************/
bool UDMCommand_MoveShip::InitializeCommand_Implementation(UDMCommandInit* InitVariables)
{
	UDMCommandInitMoveShip* InitMove = Cast<UDMCommandInitMoveShip>(InitVariables);
	if (!IsValid(InitMove) || !IsValid(InitMove->pShip))
	{
		return false;
	}
	pShip = InitMove->pShip;

	return Super::InitializeCommand_Implementation(InitVariables);
}

/******************************************************************************
 * Makes sure our ship still exists
 * returns true if command can be run successfully
******************************************************************************/
bool UDMCommand_MoveShip::Validate_Implementation() const /* override */
{
	// test our base variables
	if (!Super::Validate_Implementation())
	{
		return false;
	}

	// Its our ship, right?
	if (!IsValid(pShip) || !pOwningPlayer->TeamComponent->IsSameTeam(pShip->TeamComponent))
	{
		return false;
	}

	return true;
}

/******************************************************************************
 * returns a string with the name of the command, what it does, and what it
 *		will operate on
******************************************************************************/
FString UDMCommand_MoveShip::CommandDebug_Implementation() const /* override */
{
	return FString::Printf(TEXT("Player %s attempting to move ship %s to planet %s"),
		IsValid(pOwningPlayer) ? *pOwningPlayer->GetName() : *FString("INVALID PLAYER"),
		IsValid(pShip) ? *pShip->GetName() : *FString("INVALID SHIP"),
		IsValid(pTargetNode) ? *pTargetNode->GetName() : *FString("INVALID TARGET"));
}

/******************************************************************************
 * create a new moveship command object based on input data
******************************************************************************/
UDMCommand* UDMCommand_MoveShip::CopyCommand(const FCommandPacket& Packet) /* override */
{
	UDMCommand_MoveShip* pNewCommand = NewObject<UDMCommand_MoveShip>();
	pNewCommand->GetCopyCommandData(Packet.Data);

	return pNewCommand;
}

/******************************************************************************
 * These functions are used to fill and decode data during the CopyCommand function
******************************************************************************/
void UDMCommand_MoveShip::FillCopyCommandData(TArray<TObjectPtr<UObject>>& CommandData) /* override */
{
	Super::FillCopyCommandData(CommandData);

	CommandData.Add(pShip);
}

void UDMCommand_MoveShip::GetCopyCommandData(const TArray<TObjectPtr<UObject>>& CommandData) /* override */
{
	// Copy
	if (CommandData.Num() < 3)
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand_MoveShip::GetCopyCommandData: Data not properly instantiated, no data will be copied"))
		return;
	}
	pShip = Cast<ADMShip>(CommandData[2]);

	// Copy parent data + call validate
	Super::GetCopyCommandData(CommandData);
}
