// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMPlanet.h"		// Base Class Definition
#include "GameSettings/DMGameState.h"	// ADMGameState
#include "GameSettings/DMGameMode.h"	// EDMPlayerTeam
#include "Net/UnrealNetwork.h"			// DOREPLIFETIME
#include "Player/DMShip.h"				// ADMShip

/******************************************************************************
 * Replication
******************************************************************************/
void ADMPlanet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlanet, OwningTeam);
	DOREPLIFETIME(ADMPlanet, OwningPlayer);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Ship Management ///////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Spawn a ship on the planet!
******************************************************************************/
void ADMPlanet::K2_SpawnShip_Implementation(TSubclassOf<ADMShip> Ship, EDMPlayerTeam Team)
{
	// Can we?
	if (K2_HasShip() || Ship == nullptr)
	{
		return;
	}

	// Do it
	ADMShip* NewShip = GetWorld()->SpawnActor<ADMShip>(Ship);
	NewShip->Team = Team;
	SetCurrentShip(NewShip);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Properties and Accessors //////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * When OwningTeam changes, fire an event for everyone so that clients can
 * handle things like color changes!
******************************************************************************/
void ADMPlanet::OnRep_OwningTeam()
{
	OnOwningTeamChanged.Broadcast(this, OwningTeam);
}

/******************************************************************************
 * ADMGalaxyMode override; account for player ownership
******************************************************************************/
void ADMPlanet::SetCurrentShip(ADMShip* NewShip) /* override */
{
	Super::SetCurrentShip(NewShip);

	if (!HasAuthority())
	{
		// Debug in parent
		return;
	}

	// Set the owning player and team (DMTODO: Static Gamestate Gettor)
	UWorld* pWorld = GetWorld();
	AGameStateBase* pGameStateBase = IsValid(pWorld) ? pWorld->GetGameState() : nullptr;
	ADMGameState* pGameState = Cast<ADMGameState>(pGameStateBase);
	check(pGameState)
	if (IsValid(NewShip))
	{
		OwningTeam = NewShip->Team;
	}
	else
	{
		OwningTeam = EDMPlayerTeam::Unowned;
	}

	OwningPlayer = pGameState->GetPlayerForTeam(OwningTeam);

}