// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DMCommand.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class MULTSTRAT_API UDMCommand : public UObject
{
	GENERATED_BODY()
	
public:
	virtual bool RunCommand() const;
	virtual bool InitializeCommand(ADMPlayerState* RequestingPlayer, ADMGalaxyNode* Target);
	virtual bool Validate() const;
	// Called when a command is registed with the Command Queue Subsystem
	virtual void CommandRegistered()		{}
	// Called when a command is unregisted with the Command Queue Subsystem or after it runs
	virtual void CommandUnregistered()		{}
	virtual FString CommandDebug() const	{ return ""; }

	UFUNCTION(BlueprintCallable, meta=(ToolTip="Get this Command's ID. IDs can be used to cancel commands. If 0, this command was never registered and is considered Invalid."))
	int GetID()								{ return static_cast<int>(CommandID); }
	void SetID(uint16 NewID);

	const ADMPlayerState* GetOwningPlayer() { return OwningPlayer; }
	

	// higher priority commands run first
	// set when registered to the command queue
	UPROPERTY()
	uint8 Priority = 0;

protected:
	TObjectPtr<ADMPlayerState> OwningPlayer;
	TObjectPtr<ADMGalaxyNode> TargetNode;

private:
	uint16 CommandID = 0;
};
