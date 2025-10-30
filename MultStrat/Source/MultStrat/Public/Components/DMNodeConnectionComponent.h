// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/SplineMeshActor.h"
#include "GameFramework/Actor.h"
#include "Components/ActorComponent.h"
#include "DMNodeConnectionComponent.generated.h"

class ADMGalaxyNode;
class ADMShip;
class USplineComponent;

/**
 * Connector Actor, used to draw visualization between nodes for the player
 */
UCLASS(Blueprintable)
class MULTSTRAT_API ADMConnector : public ASplineMeshActor
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction))
	void InitializeSplineMesh(const ADMGalaxyNode* StartingNode, const ADMGalaxyNode* EndingNode);

	void SetTraversingShip(ADMShip* NewShip)	{ TraversingShip = NewShip; }
	ADMShip* GetTraversingShip()				{ return TraversingShip; }

protected:
	/** Used while commands are running to check for "bounce" situations where 2 ships use the same connector */
	UPROPERTY()
	TObjectPtr<ADMShip> TraversingShip;
};

/**
 * Specific storage for a node's connections
 */
UCLASS(Blueprintable)
class MULTSTRAT_API UDMNodeConnectionComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	/** This doesn't need to tick, turn it off in constructor*/
	UDMNodeConnectionComponent(const FObjectInitializer& ObjectInitializer);

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~ Begin UActorComponent Interface

	/** On BeginPlay, Construct or find our connectors */
	virtual void BeginPlay() override;

	//~ End UActorComponent Interface

	/**
	 * Reserve a connector from this planet to another planet
	 * Returns true if the spot is reserved; returns false if the connector is in use, or if the connection DNE
	 * If this function is called while another ship has reserved the conector, we will "bounce" that ship's movement
	 */
	bool ReserveShipTraversal(ADMGalaxyNode* TargetNode, ADMShip* ReservingShip);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated)
	TArray<const ADMGalaxyNode*> ConnectedNodes;

protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TSubclassOf<ADMConnector> ConnectorClass;

	/**
	 * Find the associated connector for the planet; should be at the same index
	 * in the ConnectorSplines array as the planet is in our ConnectedNodes array
	 */
	ADMConnector* GetConnectorForNode(const ADMGalaxyNode* Planet);

	/** Array of connector splines; each connector is at the same index of its related planet in ConnectedNodes */
	TArray<ADMConnector*> ConnectorSplines;
};