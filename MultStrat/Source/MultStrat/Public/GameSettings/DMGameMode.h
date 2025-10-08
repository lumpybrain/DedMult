// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DMGameMode.generated.h"

class UDMCommand;
class ADMShip;

UENUM()
enum class EDMPlayerTeam : uint8
{
	Unowned = 0,
	TeamOne,
	TeamTwo,
	TeamThree,
	TeamFour,
	TeamFive,
	TeamSix,
	TeamSeven,
	TeamEight,
	Count
};

UCLASS()
class MULTSTRAT_API UTeamDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TMap<EDMPlayerTeam, FColor> PlayerColors;
};

UCLASS()
class MULTSTRAT_API UCommandsDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TMap<TSubclassOf<UDMCommand>, uint8> CommandPriorities;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADMShip> DefaultShip;
};


/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:

	virtual void InitGameState() override;
	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	uint8 GetPriorityForCommand(UDMCommand* Command);
	TSubclassOf<ADMShip> GetDefaultShip();
	float GetShipSpawnZOffset()							{ return ShipSpawnZOffset; }

private:
	UPROPERTY(EditDefaultsOnly)
	uint8 MaxNumPlayers = 8;

	UPROPERTY(EditDefaultsOnly)
	float ShipSpawnZOffset = 50.0f;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTeamDataAsset> TeamDataAsset;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UCommandsDataAsset> CommandsDataAsset;
	
};
