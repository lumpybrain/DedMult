// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Commands/DMCommand.h"
#include "DMCommand_BuildShip.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API UDMCommand_BuildShip : public UDMCommand
{
	GENERATED_BODY()

public:
	// UDMCommand
	virtual bool RunCommand() const override;
	virtual bool Validate() const override;
	virtual FString CommandDebug() const override;
	virtual void CommandRegistered() override;
	virtual void CommandUnregistered() override;

};
