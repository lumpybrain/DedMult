// Fill out your copyright notice in the Description page of Project Settings.


#include "Commands/DMCommandQueueSubsystem.h"

#include "Commands/DMCommand.h"			// UDMCommand
#include "GameSettings/DMGameMode.h"	// ADMGameMode
#include "Player/DMPlayerState.h"		// ADMPlayerState

DEFINE_LOG_CATEGORY(LogCommands);

// ----------------------------------------------------------------------------
// https://forums.unrealengine.com/t/subsystem-only-on-server/645021/5
bool UDMCommandQueueSubsystem::ShouldCreateSubsystem(UObject* Outer) const /* override */
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	UWorld* World = Cast<UWorld>(Outer);
	// will be created in standalone, dedicated servers, and listen servers
	return World->GetNetMode() < NM_Client; 
}

// ----------------------------------------------------------------------------
void UDMCommandQueueSubsystem::ExecuteCommands()
{
	// Reset Command IDs
	CommandNumForTurn = 0;

	// Higher priority commands first
	ActiveCommands.Sort([](const UDMCommand& first, const UDMCommand& second)
	{
		return first.Priority > second.Priority;
	});

	// Run + Debug print
	for (UDMCommand* Command : ActiveCommands)
	{
		if (!IsValid(Command) || !Command->Validate())
		{
			UE_LOG(LogCommands, Warning, TEXT("Command %s tried to run but has been invalidated since its registration"), IsValid(Command) ? *Command->GetName() : *FString("NULLCLASS"))
			continue;
		}
		FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)Command->GetOwningPlayer()->CurrTeam);
		UE_LOG(LogCommands, Display, TEXT("Executing Command from player %s on team %s: %s"), *Command->GetOwningPlayer()->GetName(), *EnumName, *Command->CommandDebug())
		Command->RunCommand();
		Command->CommandUnregistered();
	}

	ActiveCommands.Empty();
}

// ----------------------------------------------------------------------------
int UDMCommandQueueSubsystem::RequestCommand(UDMCommand* Command)
{
	if (!IsValid(Command))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RequestCommand: Null Command Requested!"))
		return 0;
	}

	if (!Command->Validate())
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RequestCommand: Command Invalid! (Was it initialized properly?)"))
		return 0;
	}

	// get the priority for the command set properly
	UWorld* World = GetWorld();
	ADMGameMode* GameMode = IsValid(World) ? Cast<ADMGameMode>(World->GetAuthGameMode()) : nullptr;
	
	ensure(IsValid(GameMode));
	if (IsValid(GameMode))
	{
		Command->Priority = GameMode->GetPriorityForCommand(Command);
	}


	// register
	Command->SetID(CommandNumForTurn);
	++CommandNumForTurn;

	ActiveCommands.Add(Command);
	Command->CommandRegistered();

	UE_LOG(LogCommands, Display, TEXT("Command for player %s registered (%s)"), *Command->GetOwningPlayer()->GetName(), *Command->CommandDebug())

	return Command->GetID();
}

// ----------------------------------------------------------------------------
bool UDMCommandQueueSubsystem::CancelCommand(const ADMPlayerState* Player, int CommandID)
{
	// Remove a command if it is found, and if the right player owns it
	for (int i = 0; i < ActiveCommands.Num(); ++i)
	{
		if (!ActiveCommands[i]->Validate())
		{
			continue;
		}
		if (ActiveCommands[i]->GetID() == CommandID)
		{
			const ADMPlayerState* CommandPlayer = ActiveCommands[i]->GetOwningPlayer();
			if (ActiveCommands[i]->GetOwningPlayer()->GetPlayerId() != Player->GetPlayerId())
			{
				UE_LOG(LogCommands, Warning, TEXT("Player %s just tried to cancel Command Key %d, owned by player %s! (Command: %s)"),
					   *Player->GetName(), CommandID, *CommandPlayer->GetName(), *ActiveCommands[i]->CommandDebug())
				
				return false;
			}
			else
			{
				UE_LOG(LogCommands, Warning, TEXT("Player %s just cancelled Command %s "), *CommandPlayer->GetName(),
					*ActiveCommands[i]->CommandDebug())

				ActiveCommands[i]->CommandUnregistered();
				ActiveCommands.RemoveAt(i);
				return true;
			}
		}
	}


	UE_LOG(LogCommands, Error, TEXT("%s tried to cancel command ID %d, which does not exist!"), *Player->GetName(), CommandID)
	return false;
}