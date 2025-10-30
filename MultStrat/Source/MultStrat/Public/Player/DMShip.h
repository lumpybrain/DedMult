// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GalaxyObjects/DMBaseGalaxyObject.h"
#include "DMShip.generated.h"

class ADMGalaxyNode;
class ADMPlayerState;

/**
 * Base class for ships players will use to conquer or collect resources
 */
UCLASS()
class MULTSTRAT_API ADMShip : public ADMBaseGalaxyObject
{
	GENERATED_BODY()
	
public:
	/** Constructor: enable replication */
	ADMShip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** 
	 * Get the current galaxy node the ship is docked at. 
	 * Note: May be nullptr if commands are currently executing. 
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ADMGalaxyNode* GetCurrentNode() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, 
		meta = (ToolTip = "Returns true if a target node is reachable in one movement command.\nNote: Returns false if the target node is the ships current node."))
	bool IsNodeReachable(const ADMGalaxyNode* TargetNode) const;
	virtual bool IsNodeReachable_Implementation(const ADMGalaxyNode* TargetNode) const;

	UFUNCTION(BlueprintCallable)
	ADMPlayerState* GetOwningPlayer()					{ return OwningPlayer; }
	UFUNCTION(BlueprintCallable)
	const ADMPlayerState* GetOwningPlayerConst() const	{ return OwningPlayer; }
	void SetOwningPlayer(ADMPlayerState* NewOwner)		{ OwningPlayer = NewOwner; }

	int GetShipPower() const							{ return ShipPower;}

protected:

	/** Player that owns the ship for debug/possibility of alliances in the future */
	UPROPERTY(BlueprintReadWrite, Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer;

	/** Power of the ship used for Attacking/Supporting other nodes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int ShipPower = 1;
	
};
