// Copyright (c) 2025 William Pritz under MIT License


#include "GameSettings/DMGameState.h"

#include "Commands/DMCommandQueueSubsystem.h"			// UDMCommandQueueSubsystem
#include "Components/DMTeamComponent.h"					// UDMTeamComponent, EDMPlayerTeam
#include "GalaxyObjects/DMPlanetProcessingSubsystem.h"	// UDMPlanetProcessingSubsystem
#include "GameFramework/PlayerState.h"					// APlayerState
#include "GameSettings/DMGameMode.h"					// UTeamDataAsset
#include "Net/UnrealNetwork.h"							// DOREPLIFETIME
#include "Player/DMBaseController.h"					// ADMPlayerState
#include "Player/DMPlayerState.h"						// ADMPlayerState

/******************************************************************************
 * Replication
******************************************************************************/
void ADMGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMGameState, CurrentTeamData);
	DOREPLIFETIME(ADMGameState, NextNewTeam);
	DOREPLIFETIME(ADMGameState, bTurnProcessing);

}

/******************************************************************************
 * Static Gettor
******************************************************************************/
ADMGameState* ADMGameState::Get(UObject* WorldContextObject)
{
	UWorld* pWorld = WorldContextObject != nullptr ? WorldContextObject->GetWorld() : nullptr;
	AGameStateBase* pState = pWorld != nullptr ? pWorld->GetGameState() : nullptr;
	ADMGameState* pDMState = Cast<ADMGameState>(pState);

	return pDMState;
}

/*/////////////////////////////////////////////////////////////////////////////
*	Active Player Management //////////////////////////////////////////////////
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
	DMPlayerState->TeamComponent->SetTeam(NextNewTeam);
	NextNewTeam = (EDMPlayerTeam)((uint8)NextNewTeam + 1);
}

/******************************************************************************
 * Remove the players team
******************************************************************************/
void ADMGameState::UnregisterPlayerState(APlayerState* PlayerState) /* override */
{
	// DMTODO: Should all owned objects by a player go to unowned?

	ADMPlayerState* DMPlayerState = Cast<ADMPlayerState>(PlayerState);
	if (PlayerState == nullptr)
	{
		return;
	}

	DMPlayerState->TeamComponent->SetTeam(EDMPlayerTeam::Unowned);
}

/******************************************************************************
 * Check for all player states to see if they've submitted their turns
 * If they have, execute the turn
 * 
 * Server Function
******************************************************************************/
void ADMGameState::CheckAllPlayersTurnsSubmitted_Implementation()
{
	// mark turn is processing
	bTurnProcessing = true;

	for (int i = 0; i < PlayerArray.Num(); ++i)
	{

		// if we find an active commander who doesn't have a turn submitted,
		// don't try to process the turn

		// DMTODO: Ignore dead/out players
		if (ADMPlayerState* pPlayer = Cast<ADMPlayerState>(PlayerArray[i]))
		{
			if (!pPlayer->GetTurnSubmitted())
			{

				bTurnProcessing = false;
				return;
			}
		}
	}
	
	// Execute
	UDMCommandQueueSubsystem* CommandQueue = UDMCommandQueueSubsystem::Get(this);
	ensure(CommandQueue);
	CommandQueue->ExecuteCommandsForTurn();

	// DMTODO: Maybe make Execute Commands a latent action and have this execute whenever it finishes?
	// Not really necessary for the light weight, but would be good practice
	for (int i = 0; i < PlayerArray.Num(); ++i)
	{
		if (ADMPlayerState* pPlayer = Cast<ADMPlayerState>(PlayerArray[i]))
		{
			pPlayer->SetTurnSubmitted(false);
			if (ADMBaseController* pController = Cast<ADMBaseController>(pPlayer->GetPlayerController()))
			{
				pController->ServerFinishedProcessingTurn();
			}
		}
	}

	bTurnProcessing = false;
}

/*/////////////////////////////////////////////////////////////////////////////
*	Player Metadata Management ////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Determines color for a player based on their current team
******************************************************************************/
void ADMGameState::GetColorForPlayer(APlayerState* PlayerState, FColor& Output)
{
	ADMPlayerState* DMPlayerState = Cast<ADMPlayerState>(PlayerState);
	if (!IsValid(DMPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameState::GetColorForPlayer: called on a non-deadmult player state!"))
		Output = FColor::White;
		return;
	}

	GetColorForTeam(DMPlayerState->TeamComponent->GetTeam(), Output);
}

/******************************************************************************
 * Returns color for a given team
******************************************************************************/
void ADMGameState::GetColorForTeam(EDMPlayerTeam Team, FColor& Output)
{
	if (!IsValid(CurrentTeamData))
	{
		UE_LOG(LogTemp, Error, TEXT("ADMGameState::GetColorForTeam: Gamestate does not have CurrentTeamData initialized properly!"))
			Output = FColor::White;
		return;
	}

	FColor* TeamColor = CurrentTeamData->PlayerColors.Find(Team);

	// if the color DNE, print white
	if (TeamColor != nullptr)
	{
		Output = *TeamColor;
	}
	else
	{
		FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)Team);
		UE_LOG(LogTemp, Error, TEXT("ADMGameState::GetColorForTeam: No color exists for team %s"), *EnumName)
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
		if (pPlayer->TeamComponent->GetTeam() == Team)
		{
			return pPlayer;
		}
	}

	return nullptr;
}

/******************************************************************************
 * When we're not longer processing a turn, tell the PlanetProcessingSubsystem
 *		to get to work
******************************************************************************/
void ADMGameState::OnRep_TurnProcessing()
{
	if (bTurnProcessing)
	{
		return;
	}

	UDMPlanetProcessingSubsystem* pPlanetProcessing = UDMPlanetProcessingSubsystem::Get(this);
	ensure(pPlanetProcessing);

	pPlanetProcessing->StartProcessingPlanetResults();
}
