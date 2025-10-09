// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DMGameState.generated.h"

class ADMPlayerState;
class UDMCommand;
class UTeamDataAsset;
enum class EDMPlayerTeam : uint8;

/**
 * DMGameState is responsible for managing players and their teams
 * 
 * DMTODO: DMGameState should contain information on all previous turn's for
 * players to do local rewinds
 */
UCLASS()
class MULTSTRAT_API ADMGameState : public AGameState
{
	GENERATED_BODY()

public:
	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Player Management

	/** Set the players new team */
	virtual void RegisterPlayerState(APlayerState* PlayerState);

	/** Remove the players team */
	virtual void UnregisterPlayerState(APlayerState* PlayerState);

	//~=============================================================================
	// Properties and Accessors

	/** Determines color for a player based on their current team */
	UFUNCTION(BlueprintCallable)
	void GetColorForPlayer(APlayerState* PlayerState, FColor& Output);

	/** 
	 * Checks registered players for a one who is on the given team 
	 * returns the player if found, nullptr otherwise
	 */
	UFUNCTION(BlueprintCallable)
	ADMPlayerState* GetPlayerForTeam(EDMPlayerTeam Team);

	/** Pointer to team data asset, initialized from ADMGameMode on startup */
	UPROPERTY(Transient, Replicated)
	TObjectPtr<UTeamDataAsset> CurrentTeamData;

	/** When a player joins, they will be given this team */
	UPROPERTY(Replicated)
	EDMPlayerTeam NextNewTeam;
	
private:
};
