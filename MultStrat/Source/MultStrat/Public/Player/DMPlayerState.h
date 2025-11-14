// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DMPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerTurnProcessed);

class ADMGalaxyNode;
class ADMPlayerState;
class ADMShip;
enum class EDMPlayerTeam : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FShipPowerChanged, ADMPlayerState*, Player, int, CurrentPower, int, MaxPower);

/**
 * Stores information on the current state of the player's turn submission and team
 */
UCLASS()
class MULTSTRAT_API ADMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	ADMPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<class UDMTeamComponent> TeamComponent;

	//~=============================================================================
	// Turn Functions

	/** Get whether the controller submitted its turn */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetTurnSubmitted() const				{ return bTurnSubmitted; }
	void SetTurnSubmitted(bool IsSubmitted)		{ bTurnSubmitted = IsSubmitted; }

	/** Callback for the client to unlock logic after the server has finished processing a turn */
	UFUNCTION(Client, Reliable)
	void TurnProcessed();
	void TurnProcessed_Implementation();

	/** 
	 * Event broadcasted locally on the clients to let them know the server has 
	 * finished processing a turn
	 */
	UPROPERTY(BlueprintAssignable)
	FPlayerTurnProcessed OnTurnProcessed;

	//~=============================================================================
	// Ship Management

	/** Event broadcasted when is a ship is registered/unregistered or our max power changes */
	UPROPERTY(BlueprintAssignable)
	FShipPowerChanged OnPowerChanged;

	/** 
	 * Get the total power of all registered ships (does not include queued commands)
	 */
	UFUNCTION(BlueprintCallable)
	int GetTotalShipPower() const;

	/** Players fleets can only total this much power; new build commands will fail */
	UFUNCTION(BlueprintCallable)
	int GetMaxShipPower() const { return CurrentMaxShipPower; }

	/** 
	 * Set the max ship power of a player
	 * Should only be used when initializing a new player
     */
	void InitializeShipPowers(int StartingPower, int MaxPower);

	/** 
	 * Modify the ship power. Can pass in a positive or negative number.
	 * the cap will not be allowed to fall below the starting ship power number, or above
	 *		the max allowed ship power as determined by the game mode
	 */
	UFUNCTION(BlueprintCallable)
	void ModifyMaxShipPower(int Difference);

	/** 
	 * Called by ships when they're first created so we can hook into their events
	 * Note, this should only be called on the server. We do a check to make sure.
	 */
	void RegisterNewShip(ADMShip* NewShip);


protected:

	/** callback to listen to if a ship has changed team */
	UFUNCTION()
	void ShipChangedTeam(AActor* ChangedActor, EDMPlayerTeam NewTeam);
	/** callback to listen to if a ship is destroyed */
	UFUNCTION()
	void ShipIsNoLongerOurs(AActor* DestroyedShip, EEndPlayReason::Type EndPlayReason);

	/** Trigger this boolean when we've submitted or cancelled our turn locally */
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bTurnSubmitted = false;

	/** 
	 * All of our registered ships 
	 * Note; maybe we'll do some wacky commands in the future so this wont be const?
	 */
	UPROPERTY(BlueprintReadOnly, Replicated)
	TArray<TObjectPtr<const ADMShip>> RegisteredShips;

	UPROPERTY(Replicated)
	int StartingMaxPower = 0;

	UPROPERTY(Replicated)
	int CurrentMaxShipPower = 0;

	UPROPERTY(Replicated)
	int MaxAllowedShipPower = 0;

};
