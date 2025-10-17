// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMGalaxyNode.generated.h"

class ADMGalaxyNode;
class ADMShip;
class UDMCommand;

DECLARE_LOG_CATEGORY_EXTERN(LogGalaxy, Log, All);

/**
 * Specific storage for a node's connections
 */
UCLASS()
class MULTSTRAT_API UDMNodeConnectionManager : public UActorComponent
{
	GENERATED_BODY()
public:
	/** This doesn't need to tick, turn it off in constructor*/
	UDMNodeConnectionManager();

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	TArray<ADMGalaxyNode*>	ConnectedNodes;

	// DMTODO: draw connections on begin play
};

/**
 * Galaxy Nodes make up the map of all ndoes players can interact with
 */
UCLASS()
class MULTSTRAT_API ADMGalaxyNode : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	/** Constructor: Enable Replication */
	ADMGalaxyNode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/** Replication */
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Command Functions

	/** Resolve all pending ships after all the commands have executed for a turn */
	void ResolveTurn();

	/**
	 * Used by a team to register a ship is incoming
	 * Only matters to the local player, so they can't do multiple commands to send a ship to one node
	 */
	bool SetIncomingShip(bool Incoming, const UDMCommand* OwningCommand);

	/** 
	 * Used to remove the current ship
	 * Can also be called by planet code (i.e during collapse) to free the ship from its grip
	 * 
	 * DMTODO: Ships should track if they're registered to a planet and 
	 * call this or something similar if manually destroyed!
	 */
	virtual void RemoveShip(const UDMCommand* OwningCommand = nullptr);

	/** Used by commands to register a ship trying to move to this planet for turn */
	bool AddPendingShip(ADMShip* NewShip, bool IsSupportingTeam, const UDMCommand* OwningCommand);

	//~=============================================================================
	// Properties and Accessors

	UFUNCTION(BlueprintCallable)
	bool HasShip() const											{ return CurrentShip != nullptr; }

	UFUNCTION(BlueprintCallable)
	ADMShip* GetShip() const										{ return CurrentShip; }

	/**
	 * Seperate out blueprint and C++ versions just because blueprints dont let us do pointers,
	 * and debug output is optional. C++ can just call whichever function it wants.
	 */
	UFUNCTION(BlueprintCallable)
	bool K2_HasIncomingShip(FString& OutOwningCommandDebug) const;
	bool HasIncomingShip() const									{ return IncomingShip; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
	const UDMNodeConnectionManager* GetConnectionManager() const	{ return ConnectionManagerComponent; }
	
protected:

	/**
	 * A ship has moved here, conquered here, been built here, etc.
	 * Physically move it and claim it.
	 */
	virtual void SetCurrentShip(ADMShip* NewShip);

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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDMNodeConnectionManager> ConnectionManagerComponent;

	/** Ships trying to move to this node this turn */
	TSet<TPair<TObjectPtr<ADMShip>, bool>> PendingShips;

	/**
	 * A ship wants to come to this node (i.e, build or move command is in the queue)
	 * Only one allowed so that a client can't illegally try to move 2 ships to one node
	 * 
	 * DMTODO: This is a bool related to a specific command. Should this be some
	 * kind of bitfield, as commands get more complicated, they just use the spot
	 * in the bitfield that other commands they're entangled to should be using?
	 * i.e, right now this only relates to Build and Move commands
	 */
	bool IncomingShip = false;
	TObjectPtr<const UDMCommand> IncomingShipCommand = nullptr;
};
