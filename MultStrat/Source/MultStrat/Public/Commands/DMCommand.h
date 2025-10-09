// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DMCommand.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;

/**
 * Parent Class of all commands players used to change the current gamestate
 * Run only by Command Queue Subsystem.
 * 
 * DMTODO: 
 * 
 * Simulate Commands for local players testing strategies
 * 
 * Make friend class of CommandQueueSubsystem so protect RunCommand and other functions, 
 * or leave functions public for simulation later?
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class MULTSTRAT_API UDMCommand : public UObject
{
	GENERATED_BODY()
	
public:
	//~=============================================================================
	// Command Queue Subsystem Functions

	/**
	 * Command executes on its target; presumed to be only be run by CommandQueueSubsystem
	 * returns true if command executes successfully
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool RunCommand() const;
	virtual bool RunCommand_Implementation() const;

	/** Called when a command is registed with the Command Queue Subsystem */
	UFUNCTION(BlueprintNativeEvent)
	void CommandRegistered();
	virtual void CommandRegistered_Implementation() {}

	/** Called when a command is unregisted with the Command Queue Subsystem or after it runs */
	UFUNCTION(BlueprintNativeEvent)
	void CommandUnregistered();
	virtual void CommandUnregistered_Implementation() {}

	UFUNCTION(BlueprintCallable, meta = (ToolTip = "Get this Command's ID. IDs can be used to cancel commands. If 0, this command was never registered and is considered Invalid."))
	int GetID() { return static_cast<int>(CommandID); }
	void SetID(uint16 NewID);

	//~=============================================================================
	// Command Management Functions

	/**
	 * Command Initializes the command's target and owning player; those targets may have flags modified by the command.
	 * returns true if the input is valid
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool InitializeCommand(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target);
	virtual bool InitializeCommand_Implementation(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target);

	/**
	 * Overrideable IsValid method; makes sure the command can still execute in the current gamestate
	 * returns true if command can be run successfully
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool Validate() const;
	virtual bool Validate_Implementation() const;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FString CommandDebug() const;
	virtual FString CommandDebug_Implementation() const	{ return FString::Printf(TEXT("No Debug for class %s"), *GetClass()->GetFName().ToString()); }
	
	//~=============================================================================
	// Properties and Accessors

	UFUNCTION(BlueprintCallable)
	const ADMPlayerState* GetOwningPlayer() { return pOwningPlayer; }

	/** higher priority commands run first.Set when registered to the command queue */
	UPROPERTY()
	uint8 Priority = 0;

protected:
	/** Player that owns the command; mostly used to determine team */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Owning Player"))
	TObjectPtr<const ADMPlayerState> pOwningPlayer;

	/** Galaxy node the command wants to interact with(i.e, spawning on a node, moving to a node) */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Target Node"))
	TObjectPtr<ADMGalaxyNode> pTargetNode;

private:
	/** ID managed by command queue subsystem */
	uint16 CommandID = 0;
};
