// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSettings/DMGameMode.h"

#include "GameSettings/DMGameState.h"	// ADMGameState
#include "Commands\DMCommand.h"			// UDMCommand


/******************************************************************************
 * AGameModeBase Override
 * 
 * Set up ADMGameState too 
******************************************************************************/
void ADMGameMode::InitGameState() /* override */
{
	Super::InitGameState();

	if (ADMGameState* Currstate = Cast<ADMGameState>(GameState))
	{
		Currstate->CurrentTeamData = TeamDataAsset;
		Currstate->NextNewTeam = EDMPlayerTeam::TeamOne;
	}
}

/******************************************************************************
 * AGameModeBase Override
 *
 * Check max # of players before allowing a login
******************************************************************************/
void ADMGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) /* override */
{
	if (GetNumPlayers() >= MaxNumPlayers)
	{
		ErrorMessage = "Max Players Reached!";
		return;
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

/******************************************************************************
 * AGameModeBase Override
 *
 * Register the new player to the ADMGameState
******************************************************************************/
void ADMGameMode::PostLogin(APlayerController* NewPlayer) /* override */
{
	Super::PostLogin(NewPlayer);

	if (ADMGameState* CurrState = Cast<ADMGameState>(GameState))
	{
		CurrState->RegisterPlayerState(NewPlayer->PlayerState);
	}
}

/******************************************************************************
 * AGameMode Override
 *
 * Unregister the player from the ADMGameState
******************************************************************************/
void ADMGameMode::Logout(AController* Exiting) /* override */
{
	if (ADMGameState* CurrState = Cast<ADMGameState>(GameState))
	{
		CurrState->UnregisterPlayerState(Exiting->PlayerState);
	}

	Super::Logout(Exiting);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Properties and Accessors //////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Check the command data asset for a priority for the command's class
 * returns the priority if found, 0 if none found (lowest possible priority)
******************************************************************************/
uint8 ADMGameMode::GetPriorityForCommand(const TSubclassOf<UDMCommand> Command) const
{
	if (!IsValid(CommandsDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameMode::GetPriorityForCommand: Game Mode does not have a valid Command data asset!"))
		return 0;
	}

	uint8* result = CommandsDataAsset->CommandPriorities.Find(Command);

	if (result != nullptr)
	{
		return *result;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ADMGameMode::GetPriorityForCommand: Game Mode does not have a priority set for Command %s"), *Command->GetFName().ToString())
		return 0;
	}
}

/******************************************************************************
 * Check the command data asset for the default ship class
******************************************************************************/
TSubclassOf<ADMShip> ADMGameMode::GetDefaultShip() const
{
	if (!IsValid(CommandsDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameMode::GetDefaultShip: Game Mode does not have a valid Command data asset!"))
		return nullptr;
	}

	return CommandsDataAsset->DefaultShip;
}

/******************************************************************************
 * Check the Command data asset for the default ship spawn Z offset
******************************************************************************/
float ADMGameMode::GetShipSpawnZOffset() const
{
	if (!IsValid(CommandsDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameMode::GetShipSpawnZOffset: Game Mode does not have a valid Command data asset!"))
		return 0.0f;
	}

	return CommandsDataAsset->ShipSpawnZOffset;
}
