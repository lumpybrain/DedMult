// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSettings/DMGameState.h"

#include "GameFramework/PlayerState.h"	// APlayerState
#include "GameSettings/DMGameMode.h"	// EDMPlayerTeam, UTeamDataAsset
#include "Net/UnrealNetwork.h"			// DOREPLIFETIME
#include "Player/DMPlayerState.h"		// ADMPlayerState

/******************************************************************************
 * Replication
******************************************************************************/
void ADMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMGameState, CurrentTeamData);
	DOREPLIFETIME(ADMGameState, NextNewTeam);

}

/*/////////////////////////////////////////////////////////////////////////////
*	Player Management /////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Set the players new team
******************************************************************************/
void ADMGameState::RegisterPlayerState(APlayerState* PlayerState) /* override */
{
	// DMTODO: Player customization

	ADMPlayerState* DMPlayerState = Cast<ADMPlayerState>(PlayerState);
	if (DMPlayerState == nullptr)
	{
		return;
	}

	// DMTODO: Proper Team Registration!
	DMPlayerState->CurrTeam = NextNewTeam;
	NextNewTeam = (EDMPlayerTeam)((uint8)NextNewTeam + 1);
}

/******************************************************************************
 * Remove the players team
******************************************************************************/
void ADMGameState::UnregisterPlayerState(APlayerState* PlayerState) /* override */
{
	// DMTODO: Should all owned objects by a player go to unowned?

	ADMPlayerState* DMPlayerState = Cast< ADMPlayerState>(PlayerState);
	if (PlayerState == nullptr)
	{
		return;
	}

	DMPlayerState->CurrTeam = EDMPlayerTeam::Unowned;
}

/*/////////////////////////////////////////////////////////////////////////////
*	Properties and Accessors //////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Determines color for a player based on their current team
******************************************************************************/
void ADMGameState::GetColorForPlayer(APlayerState* PlayerState, FColor& Output)
{
	ADMPlayerState* DMPlayerState = Cast<ADMPlayerState>(PlayerState);
	if (!IsValid(DMPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: GetColorForPlayer called on a non-deadmult player state!"))
		Output = FColor::White;
		return;
	}
	if (!IsValid(CurrentTeamData))
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Gamestate does not have CurrentTeamData initialized properly!"))
		Output = FColor::White;
		return;
	}

	FColor* TeamColor = CurrentTeamData->PlayerColors.Find(DMPlayerState->CurrTeam);

	// if the color DNE, print white
	if (TeamColor != nullptr)
	{
		Output = *TeamColor;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Player %s attempted to find its team color. but no color exists for team %d"), *DMPlayerState->GetName(), DMPlayerState->CurrTeam)
		Output = FColor::White;
	}
}

/******************************************************************************
 * Checks registered players for a one who is on the given team
 * returns the player if found, nullptr otherwise
******************************************************************************/
ADMPlayerState* ADMGameState::GetPlayerForTeam(EDMPlayerTeam Team)
{
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		ADMPlayerState* pPlayer = Cast<ADMPlayerState>(PlayerArray[i]);
		if (pPlayer->CurrTeam == Team)
		{
			return pPlayer;
		}
	}

	return nullptr;
}
