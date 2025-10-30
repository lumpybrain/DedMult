// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DMBaseController.generated.h"


class UDMCommand;
class ADMGameState;
struct FCommandPacket;

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

	/** Queue commands in our local command list for the turn */
	UFUNCTION(BlueprintCallable)
	void QueueCommand(UDMCommand* Command);

	/** 
	 * Cancel a command in our local command list 
	 * returns true if command is canceled. 
	 * May return false if turn is submitted to server or the command DNE
	 */
	UFUNCTION(BlueprintCallable)
	bool CancelCommand(UDMCommand* Command);
	
	/**
	 * Queue a Command in the Command Queue Subsystem
	 * Run on the server because thats where the subsystem is
	 *
	 * DMTODO: Client simulation means we need this to not be a server command, but set up the game
	 * to simulate in case we are not on the server!
	 */
	UFUNCTION(Reliable, Server)
	void QueueCommandsOnServer(const TArray<FCommandPacket>& CommandInfo);
	void QueueCommandsOnServer_Implementation(const TArray<FCommandPacket>& CommandInfo);

	//~=============================================================================
	// Turn Management

	/** Called by UI/The player when they want to mark their turn as finished */
	UFUNCTION(BlueprintCallable)
	void SubmitTurn();

	/** Should only be called on by server, utility function to kick us up to the game state */
	UFUNCTION(Reliable, Server)
	void ProcessSubmittedTurn();
	virtual void ProcessSubmittedTurn_Implementation();

	UFUNCTION(Client, Reliable)
	void ServerFinishedProcessingTurn();
	void ServerFinishedProcessingTurn_Implementation();

	//UFUNCTION(BlueprintCallable, BlueprintPure)
	// bool GetTurnProcessing() const; Note: processing turn UI pop up should nullify the need for this?

	/**
	 * Called by UI/The player when they want to takesies backsies making a turn
	 * may be blocked if a turn's actions are in the middle of being processed.
	 */
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void CancelTurn();
	void CancelTurn_Implementation();

	/**
	 * Called by the server when it has finished wiping its commands to tell the client its ok to
	 * access its command list again
	 */
	UFUNCTION(Reliable, Client)
	void CommandsCancelled();
	void CommandsCancelled_Implementation();

protected:

	/** grabs the game state, makes sure that we're allowed to submit a turn */
	ADMGameState* VerifyTurnAllowed();

	/** 
	 * local array used for storage before submitting our commands
	 * must be UPROPERTY so TObjectPtr stops garbage collection
	 */
	UPROPERTY()
	TSet<TObjectPtr<UDMCommand>> CommandsForTurn;

	/** 
	 * local bool for if we have attempted to submit our turn
	 * may differ from bool in player state, which is replicated back to us
	 * after the server has received and processed our turn submission
	 */
	bool bTurnSubmittedToServer = false;
	

};
