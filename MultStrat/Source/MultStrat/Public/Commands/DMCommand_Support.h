// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand_MoveShip.h"
#include "DMCommand_Support.generated.h"

/**
 * 
 */
UCLASS()
class MULTSTRAT_API UDMCommand_Support : public UDMCommand_MoveShip
{
	GENERATED_BODY()
	
	//~ Begin UDMCommand Interface

	/** Set our ship to Support */
	virtual bool RunCommand_Implementation() override;

	/** returns a string with the name of the command, what it does, and what it will operate on */
	virtual FString CommandDebug_Implementation() const override;

	/** create a new command object based on input data */
	virtual UDMCommand* CopyCommand(const struct FCommandPacket& Packet) override;
	
	//~ End UDMCommand Interface
};
