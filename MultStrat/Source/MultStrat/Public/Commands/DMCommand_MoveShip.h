// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand.h"
#include "DMCommand_MoveShip.generated.h"

/**
 * DMTODO: Command to move a ship to a target planet (may not be adjacent in the future?)
 */
UCLASS()
class MULTSTRAT_API UDMCommand_MoveShip : public UDMCommand
{
	GENERATED_BODY()
	
//	virtual bool RunCommand() const override { return true; }

//	virtual FString CommandDebug() const { return "Move it over THERE"; }
};
