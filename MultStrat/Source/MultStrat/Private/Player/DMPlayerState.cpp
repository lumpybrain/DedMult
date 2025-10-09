// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMPlayerState.h"
#include "Commands\DMCommand.h"					// UDMCommand
#include "Commands\DMCommandQueueSubsystem.h"	// UDMCommandQueueSubsystem
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME

/******************************************************************************
 * Replication
******************************************************************************/
void ADMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlayerState, CurrTeam);
}


/******************************************************************************
 * Queue a Command in the Command Queue Subsystem
 * Run on the server because thats where the subsystem is
******************************************************************************/
void ADMPlayerState::QueueCommand_Implementation(UDMCommand* Command)
{
	UWorld* World = GetWorld();
	UDMCommandQueueSubsystem* CommandQueue = World != nullptr ? World->GetSubsystem<UDMCommandQueueSubsystem>() : nullptr;

	if (!IsValid(CommandQueue))
	{
		return;
	}

	CommandQueue->RegisterCommand(Command);
}