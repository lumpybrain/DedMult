// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GalaxyObjects/DMGalaxyNode.h"
#include "DMPlanet.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPlanetCaptured, ADMPlanet*, CapturedPlanet, EDMPlayerTeam, NewTeam);

enum class EDMPlayerTeam : uint8;
class ADMGameMode;
class ADMPlayerState;
class ADMShip;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMPlanet : public ADMGalaxyNode
{
	GENERATED_BODY()
	
public:

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Reliable, Server)
	void K2_SpawnShip(TSubclassOf<ADMShip> ShipType, EDMPlayerTeam Team);
	void K2_SpawnShip_Implementation(TSubclassOf<ADMShip> ShipType, EDMPlayerTeam Team);

	// Properties
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Replicated)
	EDMPlayerTeam OwningTeam;

	UPROPERTY(BlueprintAssignable)
	FPlanetCaptured OnCapturedDelegate;

protected:
	virtual void SetCurrentShip(ADMShip* NewShip) override;

private:
	// NOTE: The only thing i can trust here is manually replicated variables (i.e team) and player id
	UPROPERTY(Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer = nullptr;
};
