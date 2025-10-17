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

	/** Static Gettor */
	static ADMGameState* Get(UObject* WorldContextObject);

	//~=============================================================================
	// Active Player Management

	/** Set the players new team */
	virtual void RegisterPlayerState(APlayerState* PlayerState);

	/** Remove the players team */
	virtual void UnregisterPlayerState(APlayerState* PlayerState);

	/** 
	 * Check for all player states to see if they've submitted their turns
	 * If they have, execute the turn
	 */
	UFUNCTION(Reliable, Server)
	void CheckAllPlayersTurnsSubmitted();
	virtual void CheckAllPlayersTurnsSubmitted_Implementation();

	/** Gettor to know if the system is actively processing a turn */
	UFUNCTION(BlueprintCallable)
	bool IsProcessingATurn()		{ return bTurnProcessing; }

	//~=============================================================================
	// Player Metadata Management

	/** Determines color for a player based on their current team */
	UFUNCTION(BlueprintCallable)
	void GetColorForPlayer(APlayerState* PlayerState, FColor& Output);

	/** returns color for a given team */
	UFUNCTION(BlueprintCallable)
	void GetColorForTeam(EDMPlayerTeam Team, FColor& Output);

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

protected:

	/** When we're not longer processing a turn, tell the PlanetProcessingSubsystem to get to work */
	UFUNCTION()
	void OnRep_TurnProcessing();
	/** True when all the players have submitted their inputs; false after all the commands have processed */
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_TurnProcessing)
	bool bTurnProcessing;

private:
};
