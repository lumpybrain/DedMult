// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GalaxyObjects/DMGalaxyNode.h"
#include "DMPlanet.generated.h"

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

protected:

	/** ADMGalaxyMode override; account for player ownership */
	virtual void SetCurrentShip(ADMShip* NewShip) override;

private:
	/** Pointer to an owning player just in case alliances are a future feature */
	UPROPERTY(Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer = nullptr;
};
