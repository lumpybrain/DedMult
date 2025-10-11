// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DMGameMode.generated.h"

enum class EDMPlayerTeam : uint8;
class UDMCommand;
class ADMShip;

/**
 * Easy modifiable data asset for use for any data related to teams
 * I.e default team colors, max players per teams, etc
 */
UCLASS()
class MULTSTRAT_API UTeamDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TMap<EDMPlayerTeam, FColor> PlayerColors;
};

/**
 * Easy modifiable data asset for use for any data related to commands
 * I.e priorities, max # of build commands a player can do in a turn, etc.
 */
UCLASS()
class MULTSTRAT_API UCommandsDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	// Higher priority # commands will execute first when a turn executes
	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UDMCommand>, uint8> CommandPriorities;

	// Default class used when spawning a ship on a planet
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADMShip> DefaultShip;

	// Ships spawned on planets or nodes will start with this vertical offset.
	UPROPERTY(EditDefaultsOnly)
	float ShipSpawnZOffset = 50.0f;
};


/**
 * DedMults gamemode. Go to location for managing player connections and
 * variables that do not change over the course of a round of Dedmult
 */
UCLASS()
class MULTSTRAT_API ADMGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	//~ Begin AGameModeBase Interface

	/** set up ADMGameState too */
	virtual void InitGameState() override;

	/** Check max # of players before allowing a login */
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	/** Register the new player to the ADMGameState */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	//~ End AGameModeBase Interface

	//~ Begin AGameMode Interface

	/** Unregister the player from the ADMGameState */
	virtual void Logout(AController* Exiting) override;

	//~ End AGameMode Interface

	//~=============================================================================
	// Properties and Accessors

	/*
	 * Check the command data asset for a priority for the command's class
	 * returns the priority if found, 0 if none found (lowest possible priority)
	 */
	uint8 GetPriorityForCommand(const TSubclassOf<UDMCommand> Command) const;

	/** Check the command data asset for the default ship class */
	TSubclassOf<ADMShip> GetDefaultShip() const;

	/** Check the Command data asset for the default ship spawn Z offset */
	float GetShipSpawnZOffset() const;

private:

	// Max # of players allowed in this game mode
	UPROPERTY(EditDefaultsOnly)
	uint8 MaxNumPlayers = 8;

	// Default values for team-related data (i.e colors)
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTeamDataAsset> TeamDataAsset;

	// Default values for command-related data (i.e default ship to build)
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCommandsDataAsset> CommandsDataAsset;
	
};
