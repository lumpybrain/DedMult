// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DMCommandQueueSubsystem.generated.h"

class ADMPlayerState;
class UDMCommand;

DECLARE_LOG_CATEGORY_EXTERN(LogCommands, Log, All);

/**
 * Subsystem that only lives on the server (for now; may change for prediction system?)
 * Receives, tracks, cancels, and processes player commands for turn
 */
UCLASS()
class MULTSTRAT_API UDMCommandQueueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	//~ Begin UWorldSubsystem Interface

	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	//~ End UWorldSubsystem Interface

	static UDMCommandQueueSubsystem* Get(UObject* WorldContextObject);

	//~=============================================================================
	// Command Processing Functions
	
	/**
	 * Executes all commands for the turn, in priority order. Any orders submitted
	 * after this is executed will be queued for the next turn.
	 * 
	 * Any objects that want to solidify their gamestate for a turn
	 * (i.e ADMGalaxyNode::ResolveTurn) should rely on a turn being marked processed
	 * by this command
	 * 
	 * DMTODO: Keep track of the current turn # in game state
	 */
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, ToolTip = "Executes all the commands for turn. Should only be executed in C++ when all players are marked as turn submitted."))
	void ExecuteCommandsForTurn();

	/**
	 * Registers a command with the subsystem.
	 * Returns the command ID to be stored if a command is requested to be cancelled
	 */
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, ToolTip = "Note: Requesting Commands only works on the server, because the CommmandQueueSubsystem only runs on the server!\nTry using your local playerstate to request/cancel commands!"))
	int RegisterCommand(UDMCommand* Command);

	/**
	 * Unregisters a command with the subsystem.
	 * Returns if the command was succesfully cancelled (i.e, maybe the wrong player requesting cancellation)
	 */
	UFUNCTION(BlueprintCallable, meta = (DevelopmentOnly, ToolTip = "Note: Cancelling Commands only works on the server, because the CommmandQueueSubsystem only runs on the server!\nTry using your local playerstate to request/cancel commands!"))
	bool CancelCommand(const ADMPlayerState* Player, int CommandID);

private:
	/** Array of all active commands (DMTODO : Tmap.Was array becuase of replication concerns) */
	TArray<UDMCommand*> ActiveCommands;

	/** 
	 * Counter to give commands unique ID's
	 * Note: 0 is reserved for Unregistered Commands 
	 */
	uint16 CommandNumForTurn = 1;
	
};
