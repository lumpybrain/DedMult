// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSettings/DMGameMode.h"

#include "GameSettings/DMGameState.h"	// ADMGameState
#include "Commands\DMCommand.h"			// UDMCommand


// ----------------------------------------------------------------------------
void ADMGameMode::InitGameState() /* override */
{
	Super::InitGameState();

	if (ADMGameState* Currstate = Cast<ADMGameState>(GameState))
	{
		Currstate->CurrentTeamData = TeamDataAsset;
		Currstate->ShipSpawnZOffset = ShipSpawnZOffset;
		Currstate->NextNewTeam = EDMPlayerTeam::TeamOne;
	}
}
// ----------------------------------------------------------------------------
void ADMGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) /* override */
{
	if (GetNumPlayers() >= MaxNumPlayers)
	{
		ErrorMessage = "Max Players Reached!";
		return;
	}

	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

// ----------------------------------------------------------------------------
void ADMGameMode::PostLogin(APlayerController* NewPlayer) /* override */
{
	Super::PostLogin(NewPlayer);

	if (ADMGameState* CurrState = Cast<ADMGameState>(GameState))
	{
		CurrState->RegisterPlayerState(NewPlayer->PlayerState);
	}
}

// ----------------------------------------------------------------------------
void ADMGameMode::Logout(AController* Exiting) /* override */
{
	if (ADMGameState* CurrState = Cast<ADMGameState>(GameState))
	{
		CurrState->UnregisterPlayerState(Exiting->PlayerState);
	}

	Super::Logout(Exiting);
}

// ----------------------------------------------------------------------------
uint8 ADMGameMode::GetPriorityForCommand(UDMCommand* Command)
{
	if (!IsValid(CommandsDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameMode::GetPriorityForCommand: Game Mode does not have a valid Command data asset!"))
		return 0;
	}

	uint8* result = CommandsDataAsset->CommandPriorities.Find(Command->GetClass());

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

// ----------------------------------------------------------------------------
TSubclassOf<ADMShip> ADMGameMode::GetDefaultShip()
{
	if (!IsValid(CommandsDataAsset))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameMode::GetDefaultShip: Game Mode does not have a valid Command data asset!"))
		return nullptr;
	}

	return CommandsDataAsset->DefaultShip;
}
