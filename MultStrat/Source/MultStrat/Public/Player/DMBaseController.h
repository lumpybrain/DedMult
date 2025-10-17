// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DMBaseController.generated.h"


class UDMCommand;

/**
 * Base controller for all Commanders in the game
 * Useful for updating variables to turn features on/off
 *		(i.e, dont let the player use UI to submit a new move command
 *		after they've submitted for the turn!)
 */
UCLASS()
class MULTSTRAT_API ADMBaseController : public APlayerController
{
	GENERATED_BODY()

public:
	//~=============================================================================
	// Commands

	/**
	 * Queue a Command in the Command Queue Subsystem
	 * Run on the server because thats where the subsystem is
	 *
	 * DMTODO: Client simulation means we need this to not be a server command, but set up the game
	 * to simulate in case we are not on the server!
	 */
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void QueueCommand(UDMCommand* Command);
	void QueueCommand_Implementation(UDMCommand* Command);

	//~=============================================================================
	// Turn Management

	/** Called by UI/The player when they want to mark their turn as finished */
	UFUNCTION(BlueprintCallable)
	void SubmitTurn();

	/** Tell the server to check for if everyone has submitted their turn */
	UFUNCTION(Reliable, Server)
	void ProcessSubmittedTurn(ADMGameState* DMGameState);
	virtual void ProcessSubmittedTurn_Implementation(ADMGameState* DMGameState);

	UFUNCTION(Client, Reliable)
	void ServerFinishedProcessingTurn();
	void ServerFinishedProcessingTurn_Implementation();

	//UFUNCTION(BlueprintCallable, BlueprintPure)
	// bool GetTurnProcessing() const; Note: processing turn UI pop up should nullify the need for this?

	/**
	 * Called by UI/The player when they want to takesies backsies making a turn
	 * may be blocked if a turn's actions are in the middle of being processed.
	 */
	UFUNCTION(BlueprintCallable)
	void CancelTurn();

protected:


	

};
