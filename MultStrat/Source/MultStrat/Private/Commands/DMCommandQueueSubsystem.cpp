// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommandQueueSubsystem.h"

#include "Commands/DMCommand.h"			// UDMCommand
#include "GameSettings/DMGameMode.h"	// ADMGameMode, EDMPlayerTeam
#include "Player/DMPlayerState.h"		// ADMPlayerState

DEFINE_LOG_CATEGORY(LogCommands);

/******************************************************************************
 * UWorldSubsystem override; only build this subsystem on the server
 * https://forums.unrealengine.com/t/subsystem-only-on-server/645021/5
******************************************************************************/
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

/*/////////////////////////////////////////////////////////////////////////////
*	Command Processing Functions //////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Executes all the commands for turn. Should only be executed when
 *		all players are marked as turn submitted.
******************************************************************************/
void UDMCommandQueueSubsystem::ExecuteCommandsForTurn()
{
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

	// prepare for next turn
	CommandNumForTurn = 1;
	ActiveCommands.Empty();
}

/******************************************************************************
 * Registers a command with the subsystem.
 * Returns the command ID to be stored if a command is requested to be cancelled
******************************************************************************/
int UDMCommandQueueSubsystem::RegisterCommand(UDMCommand* Command)
{
	if (!IsValid(Command))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RegisterCommand: Null Command Requested!"))
		return 0;
	}
	if (!Command->Validate())
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RegisterCommand: Command Invalid! (Was it initialized properly?)"))
		return 0;
	}

	// get the priority for the command set properly
	UWorld* World = GetWorld();
	ADMGameMode* GameMode = IsValid(World) ? Cast<ADMGameMode>(World->GetAuthGameMode()) : nullptr;
	ensure(IsValid(GameMode));
	if (IsValid(GameMode))
	{
		Command->Priority = GameMode->GetPriorityForCommand(Command->GetClass());
	}


	// register with subsystem
	Command->SetID(CommandNumForTurn);
	++CommandNumForTurn;

	ActiveCommands.Add(Command);
	Command->CommandRegistered();

	UE_LOG(LogCommands, Display, TEXT("Command for player %s registered (%s)"), 
			*Command->GetOwningPlayer()->GetName(), 
			*Command->CommandDebug())

	return Command->GetID();
}

/******************************************************************************
 * Unregisters a command with the subsystem.
 * Returns if the command was succesfully cancelled 
		(i.e, maybe the wrong player requesting cancellation)
******************************************************************************/
bool UDMCommandQueueSubsystem::CancelCommand(const ADMPlayerState* Player, int CommandID)
{
	for (int i = 0; i < ActiveCommands.Num(); ++i)
	{
		if (ActiveCommands[i]->GetID() == CommandID)
		{
			const ADMPlayerState* CommandPlayer = ActiveCommands[i]->GetOwningPlayer();
			
			// Is the right player trying to cancel?
			if (ActiveCommands[i]->GetOwningPlayer()->GetPlayerId() != Player->GetPlayerId())
			{
				UE_LOG(LogCommands, Warning, TEXT("Player %s just tried to cancel Command Key %d, owned by player %s! (Command: %s)"),
					   *Player->GetName(), CommandID, *CommandPlayer->GetName(), *ActiveCommands[i]->CommandDebug())
				
				return false;
			}
			// We're good; cancel it
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