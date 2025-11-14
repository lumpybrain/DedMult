// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GalaxyObjects/DMBaseGalaxyObject.h"
#include "DMGalaxyNode.generated.h"

class ADMGalaxyNode;
class ADMShip;
class UDMCommand;
class UDMNodeConnectionComponent;
enum class EDMPlayerTeam : uint8;

DECLARE_LOG_CATEGORY_EXTERN(LogGalaxy, Log, All);

/**
 * Galaxy Nodes make up the map of all ndoes players can interact with
 */
UCLASS()
class MULTSTRAT_API ADMGalaxyNode : public ADMBaseGalaxyObject
{
	GENERATED_BODY()
	
public:
	/** Constructor: Enable Replication */
	ADMGalaxyNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Command Functions

	/** 
	 * Check all pending ships and see if the math works out where no matter what happens
	 * in other combats, this node can safely resolve 
	 */
	virtual bool CanResolveTurn();

	/** Resolve all pending ships after all the commands have executed for a turn */
	virtual void ResolveTurn();

	/**
	 * A ship on this planet is allowed to move or support if and only if
	 * 1: we're not trying to move to a node where we already have an unmoving ship
	 * 2: this ship is moving to a node which is not involved in a attack
	 *		that is stronger then the node's defenses
	 */
	virtual void PreresolveMovingShips();

	/** 
	 * Used to remove the current ship
	 * Can also be called by planet code (i.e during collapse) to free the ship from its grip
	 * 
	 * DMTODO: Ships should track if they're registered to a planet and 
	 * call this or something similar if manually destroyed!
	 */
	virtual void RemoveShip(const UDMCommand* OwningCommand = nullptr);

	/**
	 * Used by commands to register a ship trying to move to this planet for turn 
	 * returns true if ship successfully registers, false otherwise
	 */
	virtual bool AddPendingShip(ADMShip* NewShip, bool IsSupportingTeam, const UDMCommand* OwningCommand);

	/** 
	 * Can be called when ships are bounced, or their movement is invalidated in some other way
	 * returns true if the ship existed in the pending ship queue, false otherwise
	 */
	virtual bool RemovePendingShip(const ADMShip* OldShip);

	/**
	 * Get all the ships currently trying to move onto this planet.
	 * Does not include supporters.
	 * Note, this is ONLY valid while commands are running, and should NOT
	 *		be queried outside of the command run loop.
	 *
	 * Returns an array of all ships trying to move to the node
	 */
	UFUNCTION(BlueprintCallable)
	void GetPendingAttackers(TArray<ADMShip*>& OutShips);

	//~=============================================================================
	// Properties and Accessors

	UFUNCTION(BlueprintCallable)
	virtual bool HasShip() const											{ return pCurrentShip != nullptr; }

	UFUNCTION(BlueprintCallable)
	virtual ADMShip* GetShip() const										{ return pCurrentShip; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDMNodeConnectionComponent* GetConnectionManager() const		{ return ConnectionManagerComponent; }
	
protected:

	/**
	 * A ship has moved here, conquered here, been built here, etc.
	 * Physically move it and claim it.
	 */
	virtual void SetCurrentShip(ADMShip* NewShip);

	/**  
	 * Calculate the power of all factions attack this node
	 * Returns the team with the highest power level; Unowned if there's a tie 
	 */
	EDMPlayerTeam GetPendingPowers(TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>>& Powers);

	/** 
	 * Current ship docked at this node
	 * 
	 * Note: not accessible from blueprints so we don't try to destroy it!
	 * any changes to ships (i.e changing their color, shape, etc) should be done
	 * INSIDE the ship's class, either via code or blueprints!
	 */
	UPROPERTY(Replicated)
	TObjectPtr<ADMShip> pCurrentShip = nullptr;

	/** Compartmentalized management of connected nodes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDMNodeConnectionComponent> ConnectionManagerComponent;

	/** Ships trying to move to this node this turn */
	UPROPERTY()
	TMap<TObjectPtr<ADMShip>, bool> PendingShips;
};
