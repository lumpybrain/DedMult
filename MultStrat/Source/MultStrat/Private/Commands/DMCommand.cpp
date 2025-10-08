// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/DMCommand.h"

#include "Commands/DMCommandQueueSubsystem.h"	// LogCommands
#include "GalaxyObjects\DMGalaxyNode.h"			// ADMGalaxyNode
#include "Player/DMPlayerState.h"				// ADMPlayerState

// ----------------------------------------------------------------------------
bool UDMCommand::RunCommand() const
{ 
	UE_LOG(LogCommands, Warning, TEXT("Class %s Tried to run despite not having a written Run Command! Curse you, Unreal Abstract Classes!"), *GetClass()->GetFName().ToString())
	return false;
}

// ----------------------------------------------------------------------------
bool UDMCommand::InitializeCommand(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target)
{
	if (!IsValid(RequestingPlayer) || !IsValid(Target))
	{
		return false;
	}

	OwningPlayer = RequestingPlayer;
	TargetNode = Target;

	return true;
}

// ----------------------------------------------------------------------------
bool UDMCommand::Validate() const
{ 
	return IsValid(OwningPlayer) && IsValid(TargetNode);
}

// ----------------------------------------------------------------------------
void UDMCommand::SetID(uint16 NewID)
{
	if (CommandID != 0)
	{
		UE_LOG(LogCommands, Warning, TEXT("Log %d was assigned a new ID despite having an assigned ID (Class: %s"), CommandID, *GetClass()->GetFName().ToString())
	}

	CommandID = NewID;
}