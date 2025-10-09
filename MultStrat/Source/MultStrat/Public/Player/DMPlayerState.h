// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DMPlayerState.generated.h"

class UDMCommand;
enum class EDMPlayerTeam : uint8;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 
	 * Queue a Command in the Command Queue Subsystem
	 * Run on the server because thats where the subsystem is
	 *
	 * DMTODO: Client simulation means we need this to not be a server command, but set up the game
	 * to simulate in case we are not on the server!
	 */
	UFUNCTION(BlueprintCallable, Reliable, Server)
	void QueueCommand(UDMCommand* Command);
	void QueueCommand_Implementation(UDMCommand* Command);

	UPROPERTY(Replicated)
	EDMPlayerTeam CurrTeam;
};
