// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand.h"
#include "DMCommand_MoveShip.generated.h"

class ADMShip;

/**
 * Class used to initialize move ship commands
 * 
 * Not exposed as a blueprint types; blueprints should use the DMCommand Blueprint Library
 *		to build this class
 */
UCLASS()
class MULTSTRAT_API UDMCommandInitMoveShip : public UDMCommandInit
{
	GENERATED_BODY()

public:
	/** Ship to be moved */
	UPROPERTY(BlueprintReadWrite, meta = (DisplayName = "Ship"))
	ADMShip* pShip = nullptr;
};

/**
 * Command to move a ship to a target planet (may not be adjacent in the future?)
 */
UCLASS()
class MULTSTRAT_API UDMCommand_MoveShip : public UDMCommand
{
	GENERATED_BODY()
	
	//~ Begin UDMCommand Interface

	/** Move our target ship */
	virtual bool RunCommand_Implementation() const override;

	/** When we register, tell our target planet that a ship is incoming! */
	virtual void CommandRegistered_Implementation() override;

	/** When we unregister, tell our target planet no ship is coming anymore! */
	virtual void CommandUnregistered_Implementation() override;

	/**
	 * Initialize variables.InitVariables must be of class UDMCommandInitMoveShip.
	 * returns whether init was successful.Init can be unsuccessful if
	 * the input is not properly initialized(i.e passing in nullptrs)
	 */
	virtual bool InitializeCommand_Implementation(UDMCommandInit* InitVariables) override;

	/**
	 * Makes sure our ship still exists
	 * returns true if command can be run successfully
	 */
	virtual bool Validate_Implementation() const override;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	virtual FString CommandDebug_Implementation() const override;

	//~ End UDMCommand Interface

protected:
	/** Ship that this command wants to move. Should be owned by the command's owning player. */
	UPROPERTY(BlueprintReadOnly, meta = (DisplayName = "Ship"))
	TObjectPtr<ADMShip> pShip;
};
