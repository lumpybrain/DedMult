// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand.h"
#include "DMCommand_BuildShip.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;

/**
 * Command to build a ship on a planet; only works if the target is a planet
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BuildShip : public UDMCommand
{
	GENERATED_BODY()

public:
	//~ Begin UDMCommand Interface

	/**
	 * Build the ship on the planet!
	 * returns true if command executes successfully
	 */
	virtual bool RunCommand_Implementation() const override;

	/** When we register, tell our target planet that a ship is incoming! */
	virtual void CommandRegistered_Implementation() override;

	/** When we unregister, tell our target planet no ship is coming anymore! */
	virtual void CommandUnregistered_Implementation() override;
	
	/**
	 * Make sure our planet still exists
	 * returns true if command can be run successfully
	 */
	virtual bool Validate_Implementation() const override;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	virtual FString CommandDebug_Implementation() const override;

	//~ End UDMCommand Interface

};
