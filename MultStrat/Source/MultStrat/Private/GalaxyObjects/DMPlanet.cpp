// Fill out your copyright notice in the Description page of Project Settings.


#include "GalaxyObjects/DMPlanet.h"		// Base Class Definition
#include "GameSettings/DMGameState.h"	// ADMGameState
#include "GameSettings/DMGameMode.h"	// EDMPlayerTeam
#include "Net/UnrealNetwork.h"			// DOREPLIFETIME
#include "Player/DMShip.h"				// ADMShip

// ----------------------------------------------------------------------------
void ADMPlanet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlanet, OwningTeam);
	DOREPLIFETIME(ADMPlanet, OwningPlayer);
}

// ----------------------------------------------------------------------------
void ADMPlanet::K2_SpawnShip_Implementation(TSubclassOf<ADMShip> Ship, EDMPlayerTeam Team)
{
	if (K2_HasShip() || Ship == nullptr)
	{
		return;
	}

	ADMShip* NewShip = GetWorld()->SpawnActor<ADMShip>(Ship);
	NewShip->Team = Team;

	SetCurrentShip(NewShip);
}

// ----------------------------------------------------------------------------
void ADMPlanet::SetCurrentShip(ADMShip* NewShip) /* override */
{
	Super::SetCurrentShip(NewShip);

	if (!HasAuthority())
	{
		return;
	}

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