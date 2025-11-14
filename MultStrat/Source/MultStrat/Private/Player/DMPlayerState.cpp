// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMPlayerState.h"

#include "Commands/DMCommand_BuildShip.h"	// UDMCommand_BuildShip
#include "Components/DMTeamComponent.h"		// UDMTeamComponent, EDMPlayerTeam
#include "GalaxyObjects/DMGalaxyNode.h"		// LogGalaxy
#include "Net/UnrealNetwork.h"				// DOREPLIFETIME
#include "Player/DMBaseController.h"		// ADMBaseController
#include "Player/DMShip.h"					// ADMShip
/******************************************************************************
 * Constructor
******************************************************************************/
ADMPlayerState::ADMPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
}

/******************************************************************************
 * Replication
******************************************************************************/
void ADMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlayerState, bTurnSubmitted)
	DOREPLIFETIME(ADMPlayerState, RegisteredShips)
	DOREPLIFETIME(ADMPlayerState, StartingMaxPower)
	DOREPLIFETIME(ADMPlayerState, CurrentMaxShipPower)
	DOREPLIFETIME(ADMPlayerState, MaxAllowedShipPower)
}

/******************************************************************************
 * Callback for the client to unlock logic after the server has finished 
 *		processing a turn
******************************************************************************/
void ADMPlayerState::TurnProcessed_Implementation()
{
	OnTurnProcessed.Broadcast();
}

/******************************************************************************
 * Get the total power of all registered ships (does not include queued commands)
******************************************************************************/
int ADMPlayerState::GetTotalShipPower() const
{
	int TotalPower = 0;
	for (const ADMShip* pShip : RegisteredShips)
	{
		TotalPower += pShip->GetShipPower();
	}

	return TotalPower;
}

/******************************************************************************
 * Set the max ship power of a player
 * Should only be used when initializing a new player
******************************************************************************/
void ADMPlayerState::InitializeShipPowers(int StartingPower, int MaxPower)
{
	if (MaxAllowedShipPower != 0)
	{
		UE_LOG(LogGalaxy, Warning, TEXT("ADMPlayerState::SetMaxShipPower: New ship power set, but the player state was already initialized?"),
			*GetName())
	}

	StartingMaxPower = StartingPower;
	CurrentMaxShipPower = StartingPower;
	MaxAllowedShipPower = MaxPower;
}

/******************************************************************************
 * Modify the ship power. Can pass in a positive or negative number.
 * the cap will not be allowed to fall below the starting ship power number, or above
 *		the max allowed ship power as determined by the game mode
******************************************************************************/
void ADMPlayerState::ModifyMaxShipPower(int Difference)
{
	FString LogPrint = FString::Printf(TEXT("%s power changed from %d to %d"), *GetName(), CurrentMaxShipPower, CurrentMaxShipPower + Difference);

	CurrentMaxShipPower += Difference;

	if (CurrentMaxShipPower < StartingMaxPower)
	{
		CurrentMaxShipPower = StartingMaxPower;
		LogPrint.Append(" (Below starting max: set to %d)", StartingMaxPower);
	}
	else if (CurrentMaxShipPower > MaxAllowedShipPower)
	{
		CurrentMaxShipPower = MaxAllowedShipPower;
		LogPrint.Append(" (Above allowed max: set to %d)", MaxAllowedShipPower);
	}

	UE_LOG(LogGalaxy, Display, TEXT("%s"), *LogPrint)

	OnPowerChanged.Broadcast(this, GetTotalShipPower(), CurrentMaxShipPower);
}

/******************************************************************************
 * Called by ships when they're first created so we can hook into their events
 * Note, this should only be called on the server. We do a check to make sure.
******************************************************************************/
void ADMPlayerState::RegisterNewShip(ADMShip* NewShip)
{
	// Server only. DMTODO: What do for listen servers
	if (GetNetMode() > ENetMode::NM_ListenServer)
	{
		UE_LOG(LogGalaxy, Warning, TEXT("ADMPlayerState::RegisterNewShip: %s tried to register a new ship... on a client?"),
			*GetName())
		return;
	}
	
	// this really should only be called internally from ADMShip. If we hit this check
	// we have MASSIVELY screwed up.
	check(NewShip);

	RegisteredShips.Add(NewShip);

	NewShip->OnEndPlay.AddUniqueDynamic(this, &ADMPlayerState::ShipIsNoLongerOurs);
	NewShip->TeamComponent->OnActiveTeamChanged.AddUniqueDynamic(this, &ADMPlayerState::ShipChangedTeam);

	OnPowerChanged.Broadcast(this, GetTotalShipPower(), CurrentMaxShipPower);
}

/******************************************************************************
 * callback to listen to if a ship has changed team
******************************************************************************/
void ADMPlayerState::ShipChangedTeam(AActor* ChangedActor, EDMPlayerTeam NewTeam)
{
	if (TeamComponent->GetTeam() != NewTeam)
	{
		// take it off the list and unregister from it. EndPlay reason doesn't matter
		ShipIsNoLongerOurs(ChangedActor, EEndPlayReason::Type::Destroyed);
	}
}
/******************************************************************************
 * callback to listen to if a ship is destroyed
******************************************************************************/
void ADMPlayerState::ShipIsNoLongerOurs(AActor* pDestroyedShip, EEndPlayReason::Type EndPlayReason)
{
	ADMShip* pShip = Cast<ADMShip>(pDestroyedShip);
	check(pShip);

	RegisteredShips.Remove(pShip);

	pShip->OnEndPlay.RemoveDynamic(this, &ADMPlayerState::ShipIsNoLongerOurs);
	pShip->TeamComponent->OnActiveTeamChanged.RemoveDynamic(this, &ADMPlayerState::ShipChangedTeam);

	OnPowerChanged.Broadcast(this, GetTotalShipPower(), CurrentMaxShipPower);
}