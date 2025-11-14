// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand_Support.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "Components/DMCommandFlagsComponent.h"	// ECommandFlags
#include "GalaxyObjects/DMGalaxyNode.h"			// ADMGalaxyNode
#include "Player/DMPlayerState.h"				// ADMPlayerState
#include "Player/DMShip.h"						// ADMShip


/******************************************************************************
 * Set our ship to Support the target planet
 * returns true if command can be run successfully
******************************************************************************/
bool UDMCommand_Support::RunCommand_Implementation() /* override */
{
	if (!IsValid(pTargetNode) || !IsValid(pShip))
	{
		return false;
	}

	// if anybody is attacking us besides our target, we fail
	// DMTODO: For ships that move multiple nodes at once, they should not cancel supporting
	// if moving through our target planet, but do cancel supporting otherwise. What do?
	if (ADMGalaxyNode* pCurrentNode = pShip->GetCurrentNode())
	{
		TArray<ADMShip*> AttackingShips;
		pCurrentNode->GetPendingAttackers(AttackingShips);

		if (!AttackingShips.IsEmpty() &&
			(AttackingShips.Num() > 1 || AttackingShips[0]->GetCurrentNode() != pTargetNode))
		{
			return false;
		}
	}
	else
	{
		UE_LOG(LogCommands, Warning, TEXT("%s tried to supprt, but does not belong to a planet?"),
			*pShip->GetName())
	}

	pShip->CommandsComponent->AddCommandFlags(ECommandFlags::MovingShip);
	pTargetNode->AddPendingShip(pShip, true, this);
	pShip->CommandsComponent->RegisterCommand(this);

	return true;
}

/******************************************************************************
 * returns a string with the name of the command, what it does, and what it 
 *		will operate on
******************************************************************************/
FString UDMCommand_Support::CommandDebug_Implementation() const /* override */
{
	return FString::Printf(TEXT("Player %s attempting to Support planet %s with ship %s"),
		IsValid(pOwningPlayer) ? *pOwningPlayer->GetName() : *FString("INVALID PLAYER"),
		IsValid(pTargetNode) ? *pTargetNode->GetName() : *FString("INVALID TARGET"),
		IsValid(pShip) ? *pShip->GetName() : *FString("INVALID SHIP"));
}

/******************************************************************************
 * create a new support command object based on input data
******************************************************************************/
UDMCommand* UDMCommand_Support::CopyCommand(const FCommandPacket& Packet) /* override */
{
	UDMCommand_Support* pNewCommand = NewObject<UDMCommand_Support>();
	pNewCommand->GetCopyCommandData(Packet.Data);

	return pNewCommand;
}