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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void RequestCommand(UDMCommand* Command);
	void RequestCommand_Implementation(UDMCommand* Command);

	UPROPERTY(Replicated)
	EDMPlayerTeam CurrTeam;
	
	
};
