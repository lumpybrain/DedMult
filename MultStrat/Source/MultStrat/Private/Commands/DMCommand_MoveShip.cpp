// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_MoveShip.h"

#include "Components/DMTeamComponent.h"		// DMTeamComponent
#include "GalaxyObjects/DMGalaxyNode.h"		// ADMGalaxyNode
#include "Player/DMPlayerState.h"			// ADMPlayerState
#include "Player/DMShip.h"					// ADMShip

/*/////////////////////////////////////////////////////////////////////////////
*	UDMCommand Interface //////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * If the node is there, move to it
 * returns true if command executes successfully
******************************************************************************/
bool UDMCommand_MoveShip::RunCommand_Implementation() const /* override */
{
	if (!IsValid(pTargetNode) || !IsValid(pShip))
	{
		return false;
	}

	// remove it from its current node
	if (ADMGalaxyNode* pParent = Cast<ADMGalaxyNode>(pShip->GetCurrentNode()))
	{
		pParent->RemoveShip(this);
	}

	pTargetNode->AddPendingShip(pShip, false, this);

	return true;
}

/******************************************************************************
 * When we register, tell our target planet that a ship is incoming!
******************************************************************************/
void UDMCommand_MoveShip::CommandRegistered_Implementation() /* override */
{
	pTargetNode->SetIncomingShip(true, this);
}

/******************************************************************************
 * When we unregister, tell our target planet no ship is coming anymore!
******************************************************************************/
void UDMCommand_MoveShip::CommandUnregistered_Implementation() /* override */
{
	pTargetNode->SetIncomingShip(false, this);
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



