// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMPlanet.h"		// Base Class Definition
#include "GameSettings/DMGameState.h"	// ADMGameState
#include "Components/DMTeamComponent.h"	// EDMPlayerTeam
#include "Net/UnrealNetwork.h"			// DOREPLIFETIME
#include "Player/DMShip.h"				// ADMShip

/******************************************************************************
 * Replication
******************************************************************************/
ADMPlanet::ADMPlanet(const FObjectInitializer& ObjectInitializer)
	: Super (ObjectInitializer)
{
	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
}
/******************************************************************************
 * Replication
******************************************************************************/
void ADMPlanet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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
	if (HasShip() || Ship == nullptr)
	{
		return;
	}

	// Do it
	ADMShip* NewShip = GetWorld()->SpawnActor<ADMShip>(Ship);
	NewShip->TeamComponent->SetTeam(Team);
	SetCurrentShip(NewShip);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Properties and Accessors //////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

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
	if (IsValid(NewShip) && !TeamComponent->IsSameTeam(NewShip->TeamComponent))
	{
		EDMPlayerTeam NewTeam = TeamComponent->SetTeam(NewShip->TeamComponent->GetTeam());
		OwningPlayer = NewShip->GetOwningPlayer();
	}
}