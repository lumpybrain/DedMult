// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GalaxyObjects/DMGalaxyNode.h"
#include "DMPlanet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlanetCaptured, ADMPlanet*, CapturedPlanet, EDMPlayerTeam, NewTeam);

enum class EDMPlayerTeam : uint8;
class ADMPlayerState;
class ADMShip;

/**
 * Planets are player's strongholds throughout the galaxy, capable of production and resource processing
 */
UCLASS()
class MULTSTRAT_API ADMPlanet : public ADMGalaxyNode
{
	GENERATED_BODY()
	
public:
	// Replication
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Ship Management

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void K2_SpawnShip(TSubclassOf<ADMShip> ShipType, EDMPlayerTeam Team);
	void K2_SpawnShip_Implementation(TSubclassOf<ADMShip> ShipType, EDMPlayerTeam Team);

	//~=============================================================================
	// Properties and Accessors

	/*
	 * Team that owns the planet
	 * Sticky; should not change until the planet is interacted with by a command
	 * or conquered by an opposing player
	 * 
	 * When OwningTeam changes, fire an event for everyone so that clients can
	 * handle things like color changes!
	 */
	UFUNCTION()
	void OnRep_OwningTeam();
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, ReplicatedUsing = OnRep_OwningTeam)
	EDMPlayerTeam OwningTeam;

	/** Event for when Owning Team has changed. */
	UPROPERTY(BlueprintAssignable)
	FPlanetCaptured OnOwningTeamChanged;

protected:
	/** ADMGalaxyMode override; account for player ownership */
	virtual void SetCurrentShip(ADMShip* NewShip) override;

private:
	/** Pointer to an owning player just in case alliances are a future feature */
	UPROPERTY(Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer = nullptr;
};
