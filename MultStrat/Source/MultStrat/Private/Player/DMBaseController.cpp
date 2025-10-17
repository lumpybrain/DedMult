// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMBaseController.h"

#include "Commands/DMCommand.h"					// UDMCommand
#include "Commands/DMCommandQueueSubsystem.h"	// UDMCommandQueueSubsystem, LogCommands
#include "GameSettings/DMGameState.h"			// ADMGameState
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME
#include "Player/DMPlayerState.h"				// ADMPlayerState

/*/////////////////////////////////////////////////////////////////////////////
*	Commands //////////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Queue a Command in the Command Queue Subsystem
 * Run on the server because thats where the subsystem is
 *
 * Server Function
******************************************************************************/
void ADMBaseController::QueueCommand_Implementation(UDMCommand* Command)
{
	UDMCommandQueueSubsystem* pCommandQueue = UDMCommandQueueSubsystem::Get(this);

	if (!IsValid(pCommandQueue))
	{
		return;
	}

	pCommandQueue->RegisterCommand(Command);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Turn Management ///////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Called by UI/The player when they want to mark their turn as finished
******************************************************************************/
void ADMBaseController::SubmitTurn()
{
	ADMGameState* pDMGameState = ADMGameState::Get(this);

	if (!IsValid(pDMGameState))
	{
		UE_LOG(LogCommands, Error, TEXT("ADMPlayerState::SubmitTurn: Player %s tried to submit a turn, but couldn't get their local game state!"),
			*GetName())

			return;
	}

	if (pDMGameState->IsProcessingATurn())
	{
		UE_LOG(LogCommands, Warning, TEXT("ADMPlayerState::SubmitTurn: Player %s tried to submit a turn, but we're still processing last turn's actions!"),
			*GetName())

			return;
	}

	ProcessSubmittedTurn(pDMGameState);
}

/******************************************************************************
 * Tell the server to check for if everyone has submitted their turn
******************************************************************************/
void ADMBaseController::ProcessSubmittedTurn_Implementation(ADMGameState* pDMGameState)
{
	// We set the bool via the server so that we dont run into issues of
	// checking if all turns have been submitted before replication trickles in
	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState)
	pDMPlayerState->SetTurnSubmitted(true);

	pDMGameState->CheckAllPlayersTurnsSubmitted();
}

/******************************************************************************
 * Trigger this boolean when we've submitted or cancelled our turn locally
******************************************************************************/
void ADMBaseController::ServerFinishedProcessingTurn_Implementation()
{
	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState); 

	// make sure the bool is correct for anything listening to the event,
	// replication has not had time to execute
	pDMPlayerState->SetTurnSubmitted(false);
	pDMPlayerState->TurnProcessed();
}

/******************************************************************************
 * Called by UI/The player when they want to takesies backsies making a turn
 * May be blocked if a turn's actions are in the middle of being processed.
******************************************************************************/
void ADMBaseController::CancelTurn()
{
	ADMGameState* pDMState = ADMGameState::Get(this);

	if (!IsValid(pDMState))
	{
		UE_LOG(LogCommands, Error, TEXT("ADMPlayerState::CancelTurn: Player %s tried to cancel a turn, but couldn't get their local game state!"),
			*GetName())

			return;
	}

	if (pDMState->IsProcessingATurn())
	{
		UE_LOG(LogCommands, Warning, TEXT("ADMPlayerState::CancelTurn: Player %s tried to cancel a turn, but we're still processing last turn's actions!"),
			*GetName())

			return;
	}

	ADMPlayerState* pDMPlayerState = GetPlayerState<ADMPlayerState>();
	check(pDMPlayerState)

	pDMPlayerState->SetTurnSubmitted(false);
}
