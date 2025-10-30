// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMBaseController.h"

#include "Commands/DMCommand.h"					// UDMCommand, FCommandPacket
#include "Commands/DMCommandQueueSubsystem.h"	// UDMCommandQueueSubsystem, LogCommands
#include "GameSettings/DMGameState.h"			// ADMGameState
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME
#include "Player/DMPlayerState.h"				// ADMPlayerState

/*/////////////////////////////////////////////////////////////////////////////
*	Commands //////////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Queue commands in our local command list for the turn
******************************************************************************/
void ADMBaseController::QueueCommand(UDMCommand* pCommand)
{
	if (!IsValid(pCommand))
	{
		return;
	}

	CommandsForTurn.Add(pCommand);
	pCommand->CommandQueued();
}

/******************************************************************************
 * Cancel a command in our local command list 
 * returns true if command is canceled. 
 * May return false if turn is submitted to server or the command DNE
******************************************************************************/
bool ADMBaseController::CancelCommand(UDMCommand* pCommand)
{
	if (!IsValid(pCommand))
	{
		return false;
	}

	if (bTurnSubmittedToServer)
	{
		UE_LOG(LogCommands, Error, TEXT("ADMBaseController::CancelCommand: Player %s tried to cancel a command locally after submitting its commands to the server"),
			*GetName())
			return false;
	}

	if (CommandsForTurn.Remove(pCommand) != 0)
	{
		pCommand->CommandUnqueued();
		return true;
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 * Queue a Command in the Command Queue Subsystem
 * Run on the server because thats where the subsystem is
 *
 * Server Function
******************************************************************************/
void ADMBaseController::QueueCommandsOnServer_Implementation(const TArray<FCommandPacket>& CommandPackets)
{
	UDMCommandQueueSubsystem* pCommandQueue = UDMCommandQueueSubsystem::Get(this);

	if (!IsValid(pCommandQueue))
	{
		return;
	}

	// Make a copy of the command
	for (int i = 0; i < CommandPackets.Num(); ++i)
	{
		const FCommandPacket& CommandInfo = CommandPackets[i];

		UObject* pObjectDefault = CommandInfo.CommandClass != nullptr ? CommandInfo.CommandClass->GetDefaultObject() : nullptr;
		UDMCommand* pCommandDefault = Cast<UDMCommand>(pObjectDefault);
		if (pCommandDefault == nullptr)
		{
			UE_LOG(LogCommands, Error, TEXT("ADMBaseController::QueueCommand: Invalid command passed into QueueCommand"))
				return;
		}

		UDMCommand* pSubmittedCommand = pCommandDefault->CopyCommand(CommandInfo);

		pCommandQueue->RegisterCommand(pSubmittedCommand);
	}

	ProcessSubmittedTurn();
}

/*/////////////////////////////////////////////////////////////////////////////
*	Turn Management ///////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Called by UI/The player when they want to mark their turn as finished
******************************************************************************/
void ADMBaseController::SubmitTurn()
{
	// Make sure we're allowed to attempt to submit a turn
	ADMGameState* pDMState = VerifyTurnAllowed();
	if (pDMState == nullptr)
	{
		return;
	}

	// broadcast the data to the server
	bTurnSubmittedToServer = true;

	TArray<FCommandPacket> TurnPackets;
	int CommandCount = 0;
	for (TObjectPtr<UDMCommand> CurrCommand : CommandsForTurn)
	{
		TurnPackets.AddDefaulted();
		TurnPackets[CommandCount].InitializePacket(CurrCommand);
		++CommandCount;
	}

	QueueCommandsOnServer(TurnPackets);
}

/******************************************************************************
 * Server checks if everyone has submitted their turn
 * Server Function
******************************************************************************/
void ADMBaseController::ProcessSubmittedTurn_Implementation()
{
	// We set the bool via the server so that we dont run into issues of
	// checking if all turns have been submitted before replication trickles in
	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState)
	pDMPlayerState->SetTurnSubmitted(true);

	// Check to see if all of our players have submitted their turns
	ADMGameState* pDMState = VerifyTurnAllowed();
	if (pDMState == nullptr)
	{
		return;
	}

	pDMState->CheckAllPlayersTurnsSubmitted();
}

/******************************************************************************
 * Trigger this boolean when we've submitted or cancelled our turn locally
******************************************************************************/
void ADMBaseController::ServerFinishedProcessingTurn_Implementation()
{
	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState); 

	if (!bTurnSubmittedToServer)
	{
		UE_LOG(LogCommands, Error, TEXT("ADMBaseController::ServerFinishedProcessingTurn: Player %s got word back from the server that it finished processing the turn... but we never submitted?"),
			*pDMPlayerState->GetName())
	}

	// reset variables
	for (UDMCommand* pCommand : CommandsForTurn)
	{
		pCommand->CommandUnqueued();
	}
	bTurnSubmittedToServer = false;
	CommandsForTurn.Empty();

	// make sure the bool is correct for anything listening to the event,
	// replication has not had time to execute
	pDMPlayerState->SetTurnSubmitted(false);
	pDMPlayerState->TurnProcessed();
}

/******************************************************************************
 * Called by UI/The player when they want to takesies backsies making a turn
 * May be blocked if a turn's actions are in the middle of being processed.
 * 
 * Server Function
******************************************************************************/
void ADMBaseController::CancelTurn_Implementation()
{
	UDMCommandQueueSubsystem* pCommandQueue = UDMCommandQueueSubsystem::Get(this);
	if (!IsValid(pCommandQueue))
	{
		UE_LOG(LogCommands, Error, TEXT("ADMBaseController::CancelTurn couldn't find the Command Queue Subsystem"))
		return;
	}

	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState)

	pCommandQueue->CancelCommands(pDMPlayerState);
	pDMPlayerState->SetTurnSubmitted(false);

	// tell the client we're done
	CommandsCancelled();
}

/******************************************************************************
 * Called by the server when it has finished wiping its commands to tell the client its ok to
 * access its command list again
 * 
 * Client Function
******************************************************************************/
void ADMBaseController::CommandsCancelled_Implementation()
{
	if (!bTurnSubmittedToServer)
	{
		ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
		check(pDMPlayerState)

		UE_LOG(LogCommands, Warning, TEXT("ADMBaseController::CommandsCancelled: Player %s was told by the server it cancelled its turn, but it didn't have a turn submitted?"),
			*pDMPlayerState->GetName())
	}

	bTurnSubmittedToServer = false;
}

/******************************************************************************
 * Grabs the game state, makes sure that we're allowed to submit a turn
******************************************************************************/
ADMGameState* ADMBaseController::VerifyTurnAllowed()
{
	FString RoleChecked("(Client)");
	if (GetNetMode() < ENetMode::NM_Client)
	{
		RoleChecked = "(Server)";
	}

	ADMGameState* pDMGameState = ADMGameState::Get(this);
	if (!IsValid(pDMGameState))
	{
		UE_LOG(LogCommands, Error, TEXT("ADMBaseController %s: Player %s tried to submit a turn, but couldn't get their game state!"),
			*RoleChecked,
			*GetName())

			return nullptr;
	}

	if (pDMGameState->IsProcessingATurn())
	{
		UE_LOG(LogCommands, Warning, TEXT("ADMBaseController %s: Player %s tried to edit a turn, but we're still processing last turn's actions!"),
			*RoleChecked,
			*GetName())

			return nullptr;
	}

	return pDMGameState;
}
