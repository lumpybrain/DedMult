// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "DMPlanetProcessingSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTurnProcessingFinished);

/**
 * Used by local clients to process/animate the results of a turn
 * 
 * We use a subsystem instead of each planet operating individually so we can do
 *		things like resolving one planet at a time or resolving planets every
 *		X seconds easier.
 */
UCLASS()
class MULTSTRAT_API UDMPlanetProcessingSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:

	/** Static Gettor */
	static UDMPlanetProcessingSubsystem* Get(UObject* WorldContextObject);

	//~ Begin UWorldSubsystem Interface

	/** Disable Tick on Begin Play */
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	//~ End UWorldSubsystem Interface

	//~ Begin UTickableWorldSubsystem Interface

	/** If we're ticking, we should be processing player combat/ship movement*/
	virtual void Tick(float DeltaTime) override;

	//~ End UTickableWorldSubsystem Interface
	
	//~ Begin FTickableGameObject Interface

	virtual TStatId GetStatId() const override;

	//~ End FTickableGameObject Interface


	//~=============================================================================
	// Planet Processing Functions

	/** Called when the subsystem should start moving/animating planets */
	void StartProcessingPlanetResults();
	
protected:
	/** Let the planets start moving their respective ships to them */
	virtual void MovePendingShipsToPlanets();

	/** Let the planets resolve their combat */
	virtual void ProcessPlanetCombat();

	/** Clean up. Tell the game state we're all done processing. */
	virtual void ProcessingFinished();

	enum class EProcessingStage
	{
		MoveShips = 0,
		Combat,
	};

	EProcessingStage CurrentStage = EProcessingStage::MoveShips;

};
