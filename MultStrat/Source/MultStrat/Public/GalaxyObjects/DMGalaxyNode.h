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
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Command Functions

	/** 
	 * Check all pending ships and see if the math works out where no matter what happens
	 * in other combats, this node can safely resolve 
	 */
	bool CanResolveTurn();

	/** Resolve all pending ships after all the commands have executed for a turn */
	void ResolveTurn();

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
	bool AddPendingShip(ADMShip* NewShip, bool IsSupportingTeam, const UDMCommand* OwningCommand);

	/** 
	 * Can be called when ships are bounced, or their movement is invalidated in some other way
	 * returns true if the ship existed in the pending ship queue, false otherwise
	 */
	bool RemovePendingShip(ADMShip* NewShip);

	/**
	 * Checks with the connection component to see if the ship can traverse to the requested node
	 * returns true if successful, false otherwise
	 */
	bool ReserveTraversalTo(ADMGalaxyNode* TargetNode, ADMShip* ReservingShip);

	//~=============================================================================
	// Properties and Accessors

	UFUNCTION(BlueprintCallable)
	bool HasShip() const											{ return CurrentShip != nullptr; }

	UFUNCTION(BlueprintCallable)
	ADMShip* GetShip() const										{ return CurrentShip; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UDMNodeConnectionComponent* GetConnectionManager() const		{ return ConnectionManagerComponent; }
	
protected:

	/**
	 * A ship has moved here, conquered here, been built here, etc.
	 * Physically move it and claim it.
	 */
	virtual void SetCurrentShip(ADMShip* NewShip);

	/** Calculate the power of all factions attack this node */
	virtual void GetPendingPowers(TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>>& Powers);

	/** 
	 * Current ship docked at this node
	 * 
	 * Note: not accessible from blueprints so we don't try to destroy it!
	 * any changes to ships (i.e changing their color, shape, etc) should be done
	 * INSIDE the ship's class, either via code or blueprints!
	 */
	UPROPERTY(Replicated)
	TObjectPtr<ADMShip> CurrentShip = nullptr;

	/** Compartmentalized management of connected nodes */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UDMNodeConnectionComponent> ConnectionManagerComponent;

	/** Ships trying to move to this node this turn */
	UPROPERTY()
	TMap<TObjectPtr<ADMShip>, bool> PendingShips;
};
