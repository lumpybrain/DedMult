// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DMCommandQueueSubsystem.generated.h"

class ADMPlayerState;
class UDMCommand;

DECLARE_LOG_CATEGORY_EXTERN(LogCommands, Log, All);

/**
 * 
 */
UCLASS()
class MULTSTRAT_API UDMCommandQueueSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// UWorldSubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	// Commands
	UFUNCTION(BlueprintCallable)
	void ExecuteCommands();

	// Returns the command ID to be stored if a command is requested to be cancelled
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Note: Requesting Commands only works on the server, because the CommmandQueueSubsystem only runs on the server!\nTry using your local playerstate to request/cancel commands!"))
	int RequestCommand(UDMCommand* Command);

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Note: Cancelling Commands only works on the server, because the CommmandQueueSubsystem only runs on the server!\nTry using your local playerstate to request/cancel commands!"))
	bool CancelCommand(const ADMPlayerState* Player, int CommandID);

private:
	TArray<UDMCommand*> ActiveCommands;

	// Counter to give commands unique ID's; note, 0 is reserved for Unregistered Commands
	uint16 CommandNumForTurn = 1;
	
};
