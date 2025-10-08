// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DMPlayerState.h"
#include "Commands\DMCommand.h"					// UDMCommand
#include "Commands\DMCommandQueueSubsystem.h"	// UDMCommandQueueSubsystem
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME

// ----------------------------------------------------------------------------
void ADMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlayerState, CurrTeam);
}


// ----------------------------------------------------------------------------
void ADMPlayerState::RequestCommand_Implementation(UDMCommand* Command)
{
	UWorld* World = GetWorld();
	UDMCommandQueueSubsystem* CommandQueue = World != nullptr ? World->GetSubsystem<UDMCommandQueueSubsystem>() : nullptr;

	if (!IsValid(CommandQueue))
	{
		return;
	}

	CommandQueue->RequestCommand(Command);
}