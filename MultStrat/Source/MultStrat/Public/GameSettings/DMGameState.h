// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DMGameState.generated.h"

class ADMPlayerState;
class UDMCommand;
class UTeamDataAsset;
enum class EDMPlayerTeam : uint8;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMGameState : public AGameState
{
	GENERATED_BODY()

public:
	// Replicate
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void GetColorForPlayer(APlayerState* PlayerState, FColor& Output);

	UFUNCTION(BlueprintCallable)
	float GetDefaultShipFloatHeight() { return ShipSpawnZOffset; }

	UFUNCTION(BlueprintCallable)
	ADMPlayerState* GetPlayerForTeam(EDMPlayerTeam Team);

	/** Add PlayerState to the PlayerArray */
	virtual void RegisterPlayerState(APlayerState* PlayerState);

	/** Remove PlayerState from the PlayerArray. */
	virtual void UnregisterPlayerState(APlayerState* PlayerState);

	UPROPERTY(Transient, Replicated)
	TObjectPtr<UTeamDataAsset> CurrentTeamData;

	UPROPERTY(Transient, Replicated)
	float ShipSpawnZOffset = 50.0f;

	UPROPERTY(Replicated)
	EDMPlayerTeam NextNewTeam;
	
private:
};
