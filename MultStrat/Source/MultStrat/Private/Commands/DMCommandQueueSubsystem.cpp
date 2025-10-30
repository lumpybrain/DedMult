// Copyright (c) 2025 William Pritz under MIT License


#include "Commands/DMCommandQueueSubsystem.h"

#include "Commands/DMCommand.h"							// UDMCommand
#include "GalaxyObjects/DMPlanetProcessingSubsystem.h"	// UDMPlanetProcessingSubsystem
#include "GameSettings/DMGameMode.h"					// ADMGameMode
#include "GameSettings/DMGameState.h"					// ADMGameState
#include "Components/DMTeamComponent.h"					// EDMPlayerTeam
#include "Player/DMPlayerState.h"						// ADMPlayerState

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

	UWorld* pWorld = Cast<UWorld>(Outer);
	// will be created in standalone, dedicated servers, and listen servers
	return pWorld->GetNetMode() < NM_Client; 
}

/******************************************************************************
 * Static Gettor
******************************************************************************/
UDMCommandQueueSubsystem* UDMCommandQueueSubsystem::Get(UObject* WorldContextObject)
{
	UWorld* pWorld = WorldContextObject != nullptr ? WorldContextObject->GetWorld() : nullptr;
	UDMCommandQueueSubsystem* pQueueSubsystem = pWorld != nullptr ? pWorld->GetSubsystem<UDMCommandQueueSubsystem>() : nullptr;

	return pQueueSubsystem;
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
			UE_LOG(LogCommands, Warning, TEXT("Command %s tried to run but has been invalidated since its registration (%s)"), 
				IsValid(Command) ? *Command->GetName() : *FString("NULLCLASS"),
				IsValid(Command) ? *Command->CommandDebug() : *FString("NULLCLASS"))
			continue;
		}

		EDMPlayerTeam PlayerTeam = UDMTeamComponent::GetActorsTeam(Command->GetOwningPlayer());
		FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)PlayerTeam);
		UE_LOG(LogCommands, Display, TEXT("Executing Command from player %s on team %s: %s"), 
			*Command->GetOwningPlayer()->GetName(), 
			*EnumName, 
			*Command->CommandDebug())

		Command->RunCommand();
	}

	// prepare for next turn
	ActiveCommands.Empty();

	//ADMGameState* pDMState = ADMGameState::Get(this);
	//ensure(pDMState);

	// DMTODO: we should have this happen locally on every machine
	UDMPlanetProcessingSubsystem* CommandQueue = UDMPlanetProcessingSubsystem::Get(this);
	ensure(CommandQueue);
	CommandQueue->StartProcessingPlanetResults();
}

/******************************************************************************
 * Registers a command with the subsystem.
 * Returns the command ID to be stored if a command is requested to be cancelled
******************************************************************************/
bool UDMCommandQueueSubsystem::RegisterCommand(UDMCommand* Command)
{
	if (!IsValid(Command))
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RegisterCommand: Null Command Requested!"))
		return false;
	}
	if (!Command->Validate())
	{
		UE_LOG(LogCommands, Warning, TEXT("UDMCommandQueueSubsystem::RegisterCommand: Command Invalid! (Was it initialized properly?)"))
		return false;
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
	ActiveCommands.Add(Command);

	UE_LOG(LogCommands, Display, TEXT("Command for player %s registered (%s)"), 
			*Command->GetOwningPlayer()->GetName(), 
			*Command->CommandDebug())

	return true;
}

/******************************************************************************
 * Unregisters a command with the subsystem.
 * Returns if the command was succesfully cancelled 
		(i.e, maybe the wrong player requesting cancellation)
******************************************************************************/
void UDMCommandQueueSubsystem::CancelCommands_Implementation(const ADMPlayerState* Player)
{
	bool bFoundPlayer = false;
	size_t PlayerStartPos = 0;
	size_t PlayerEndPos = ActiveCommands.Num();
	for (size_t i = 0; i < ActiveCommands.Num() ; ++i)
	{
		if (ActiveCommands[i]->GetOwningPlayer() == Player)
		{
			if (!bFoundPlayer)
			{
				bFoundPlayer = true;
				PlayerStartPos = i;
			}

			UE_LOG(LogCommands, Display, TEXT("Player %s is trying to cancel Command %s"), 
				*Player->GetName(),
				*ActiveCommands[i]->CommandDebug())
		}
		else if (bFoundPlayer)
		{
			PlayerEndPos = i;
			break;
		}
	}

	// DMTODO turn ActiveCommands to LinkedList for cleaner removal
	if (bFoundPlayer)
	{
		ActiveCommands.RemoveAt(PlayerStartPos, PlayerEndPos - PlayerStartPos, EAllowShrinking::Yes);
		UE_LOG(LogCommands, Display, TEXT("Player %s cancelled %d Commands"),
			*Player->GetName(),
			PlayerEndPos - PlayerStartPos)
	}
	else
	{
		UE_LOG(LogCommands, Display, TEXT("Player %s tried to cancel all their commands, but had no commands queued"),
			*Player->GetName())
	}
}