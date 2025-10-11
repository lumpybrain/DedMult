// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMPlayerState.h"

#include "Commands/DMCommand.h"					// UDMCommand
#include "Commands/DMCommandQueueSubsystem.h"	// UDMCommandQueueSubsystem
#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME

/******************************************************************************
 * Constructor
******************************************************************************/
ADMPlayerState::ADMPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
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