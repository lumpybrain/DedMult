// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommand.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "GalaxyObjects\DMGalaxyNode.h"			// ADMGalaxyNode
#include "Player/DMPlayerState.h"				// ADMPlayerState

/*/////////////////////////////////////////////////////////////////////////////
*	Command Queue Subsystem Functions /////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Command executes on its target; presumed to be only be run by CommandQueueSubsystem
 * returns true if command executes successfully
******************************************************************************/
bool UDMCommand::RunCommand_Implementation() const
{ 
	UE_LOG(LogCommands, Warning, TEXT("Class %s Tried to run despite not having a written Run Command! Curse you, Unreal Abstract Classes!"), *GetClass()->GetFName().ToString())
	return false;
}

/******************************************************************************
 * ID Settor with debug
******************************************************************************/
void UDMCommand::SetID(uint16 NewID)
{
	if (CommandID != 0)
	{
		UE_LOG(LogCommands, Warning, TEXT("Command \"%s\" was assigned a new ID despite having an assigned ID (Old: %d. New: %d)"), *GetClass()->GetFName().ToString(), CommandID, NewID)
	}

	CommandID = NewID;
}

/*/////////////////////////////////////////////////////////////////////////////
*	Command Management Functions //////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Command Initializes the command's target and owning player; those targets
 *		may have flags modified by the command.
 * returns true if the input is valid
******************************************************************************/
bool UDMCommand::InitializeCommand_Implementation(UDMCommandInit* InitVariables)
{
	if (!IsValid(InitVariables) || !IsValid(InitVariables->pRequestingPlayer) 
								|| !IsValid(InitVariables->pTarget))
	{
		return false;
	}

	pOwningPlayer = InitVariables->pRequestingPlayer;
	pTargetNode = InitVariables->pTarget;

	return true;
}

/******************************************************************************
 * Overrideable IsValid method; makes sure the command can still execute in the current gamestate
 * returns true if command can be run successfully
******************************************************************************/
bool UDMCommand::Validate_Implementation() const
{
	return IsValid(pOwningPlayer) && IsValid(pTargetNode);
}

/******************************************************************************
 * Create a new UObject of this command's class type and copy the data from command data into it
 * Used when sending commands from the client to the server instead of replication so that we
 *		don't have to wait for unreal's automatic replication to copy the data over for us;
 *		not all clients need a copy of the command, just the server.
******************************************************************************/
UDMCommand* UDMCommand::CopyCommand(FCommandPacket& Packet)
{
	UDMCommand* pNewCommand = NewObject<UDMCommand>();
	pNewCommand->GetCopyCommandData(Packet.Data);

	return pNewCommand;
}

/******************************************************************************
 * These functions are used to fill and decode data during the CopyCommand function
******************************************************************************/
void UDMCommand::FillCopyCommandData(TArray<TObjectPtr<UObject>>& CommandData)
{
	CommandData.Add(pOwningPlayer);
	CommandData.Add(pTargetNode);
}

void UDMCommand::GetCopyCommandData(TArray<TObjectPtr<UObject>>& CommandData)
{
	if (CommandData.Num() < 2)
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand::GetCopyCommandData: Data not properly instantiated, no data will be copied"))
		return;
	}
	pOwningPlayer = Cast<ADMPlayerState>(CommandData[0]);
	pTargetNode = Cast<ADMGalaxyNode>(CommandData[1]);

	if (!Validate())
	{
		UE_LOG(LogCommands, Error, TEXT("UDMCommand::GetCopyCommandData: Unexpected or invalid data passed in for class %s. Check your FillCopyCommandData implementation!"),
			*GetClass()->GetName())
		return;
	}
}

void FCommandPacket::InitializePacket(UDMCommand* CommandForInfo)
{
	CommandClass = CommandForInfo->GetClass();
	CommandForInfo->FillCopyCommandData(Data);
}