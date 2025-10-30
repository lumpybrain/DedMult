// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMGalaxyNode.h"

#include "Commands/DMCommand.h"						// UDMCommand
#include "Components/DMCommandFlagsComponent.h"		// UDMActiveCommandsComponent
#include "Components/DMNodeConnectionComponent.h"	// UDMNodeConnectionComponent
#include "Components/DMTeamComponent.h"				// EDMPlayerTeam
#include "GameSettings/DMGameMode.h"				// ADMGameMode
#include "Net/UnrealNetwork.h"						// DOREPLIFETIME
#include "Player/DMShip.h"							// ADMShip

DEFINE_LOG_CATEGORY(LogGalaxy);

/******************************************************************************
 * Constructor: Enable Replication
******************************************************************************/
ADMGalaxyNode::ADMGalaxyNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	ConnectionManagerComponent = CreateDefaultSubobject<UDMNodeConnectionComponent>(TEXT("ConnectionManager"));
}

/******************************************************************************
 * Replication
******************************************************************************/
void ADMGalaxyNode::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMGalaxyNode, CurrentShip);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Command Functions /////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Check all pending ships and see if the math works out where no matter what happens
 * in other combats, this node can safely resolve
******************************************************************************/
bool ADMGalaxyNode::CanResolveTurn()
{
	// 1; No pending ships = resolvable
	if (PendingShips.IsEmpty())
	{
		return true;
	}

	// 2: Current ship on node is Not Moving or DNE  = resolvable
	ADMShip* CurrShip = GetShip();
	if (CurrShip == nullptr || !CurrShip->CommandsComponent->CheckForCommandFlags(ECommandFlags::MovingShip))
	{
		return true;
	}

	// 3; current ship Does not matter for result (win or tie for home team without it)  = resolvable
	// Map of all teams trying to take control of the planet; mapping their team to the main attacking ship and the total power of their fleet
	TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>> Powers;
	GetPendingPowers(Powers);

	// Find the winner
	size_t HighestPower = 0;
	size_t PowerDiff = 0;
	EDMPlayerTeam WinningTeam = EDMPlayerTeam::Invalid;
	for (auto TeamPower : Powers)
	{
		EDMPlayerTeam eTeam = TeamPower.Key;
		ADMShip* pShipPtr = TeamPower.Value.Key;
		size_t Power = TeamPower.Value.Value;

		if (pShipPtr == nullptr)
		{
			continue;
		}

		if (Power > HighestPower)
		{
			PowerDiff = Power - HighestPower;
			HighestPower = Power;
			WinningTeam = eTeam;
		}
		else if (Power == HighestPower)
		{
			// No winner in the case of a tie
			WinningTeam = EDMPlayerTeam::Invalid;
			PowerDiff = 0;
		}
	}
	if (WinningTeam == TeamComponent->GetTeam() &&
		PowerDiff >= CurrShip->GetShipPower())
	{
		return true;
	}

	// Not Resolvable
	return false;
}

/******************************************************************************
 * Resolve all pending ships after all the commands have executed for a turn
******************************************************************************/
void ADMGalaxyNode::ResolveTurn()
{
	// No work to be done
	if (PendingShips.IsEmpty())
	{
		return;
	}

	// Map of all teams trying to take control of the planet; mapping their team to the main attacking ship and the total power of their fleet
	TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>> Powers;
	GetPendingPowers(Powers);


	FString NodeTeam = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)TeamComponent->GetTeam());
	UE_LOG(LogGalaxy, Display, TEXT("%s combat results (Previous Owner: %s): "),
		*GetName(),
		*NodeTeam)

	// Find the winner
	size_t HighestPower = 0;
	ADMShip* WinningShip = nullptr;
	for (auto TeamPower : Powers)
	{
		EDMPlayerTeam eTeam = TeamPower.Key;
		ADMShip* pShipPtr = TeamPower.Value.Key;
		size_t Power = TeamPower.Value.Value;

		FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)eTeam);
		FString AttackDebug = FString::Printf(TEXT("	Attacked by team %s with power %d"),
			*EnumName,
			Power);

		if (pShipPtr == nullptr)
		{
			AttackDebug.Append(", But they forgot to send an attacking ship!");
			UE_LOG(LogGalaxy, Display, TEXT("%s"), *AttackDebug)
			continue;
		}

		UE_LOG(LogGalaxy, Display, TEXT("%s"), *AttackDebug)

		if (Power > HighestPower)
		{
			HighestPower = Power;
			WinningShip = pShipPtr;
		}
		else if (Power == HighestPower)
		{
			// No winner in the case of a tie
			WinningShip = nullptr;
		}
	}

	// Declare the winner!
	if (WinningShip != nullptr)
	{
		// debug
		FString WinnerEnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)WinningShip->TeamComponent->GetTeam());
		UE_LOG(LogGalaxy, Display, TEXT("	The winner is %s!"),
			*WinnerEnumName);

		if (IsValid(CurrentShip) && CurrentShip != WinningShip)
		{
			// DMTODO: Ship Retreats
			CurrentShip->Destroy();
		}

		SetCurrentShip(WinningShip);
	}
	else
	{
		UE_LOG(LogGalaxy, Display, TEXT("There is no winner!"));
	}

	// Cleanup
	PendingShips.Empty();
}

/******************************************************************************
 * Used by commands to remove the current ship
 * Can also be called by planet code (i.e during collapse) to free the ship 
 *		from its grip
******************************************************************************/
void ADMGalaxyNode::RemoveShip(const UDMCommand* OwningCommand)
{
	// Remove the current ship, being mindful if its technically in the middle of destruction/garbage collection
	if (!IsValid(CurrentShip))
	{
		UE_LOG(LogGalaxy, Warning, TEXT("%s: \"%s\" tried to remove the current ship, Current Ship is invalid."),
			*GetName(),
			IsValid(OwningCommand) ? *OwningCommand->CommandDebug() : TEXT("Manual Code Call"))
	}
	else
	{
		CurrentShip->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	CurrentShip = nullptr;
}

/******************************************************************************
 * Used by commands to register a ship trying to move to this planet for turn
******************************************************************************/
bool ADMGalaxyNode::AddPendingShip(ADMShip* NewShip, bool Supporting, const UDMCommand* OwningCommand)
{
	if (!IsValid(NewShip))
	{
		UE_LOG(LogGalaxy, Error, TEXT("%s: Invalid Pending ship added by Command \"%s\"."),
			*GetName(),
			IsValid(OwningCommand) ? *OwningCommand->CommandDebug() : TEXT("INVALID COMMAND"))

			return false;
	}

	PendingShips.Add(NewShip, Supporting);
	return true;
}

/******************************************************************************
 * Can be called when ships are bounced, or their movement is
 *		invalidated in some other way
******************************************************************************/
bool ADMGalaxyNode::RemovePendingShip(ADMShip* NewShip)
{
	return PendingShips.Remove(NewShip) != 0;
}

/******************************************************************************
 * Checks with the connection component to see if the ship can traverse to the 
 *		requested node
 * returns true if successful, false otherwise
******************************************************************************/
bool ADMGalaxyNode::ReserveTraversalTo(ADMGalaxyNode* TargetNode, ADMShip* ReservingShip)
{
	check(ConnectionManagerComponent)
	if(!IsValid(ConnectionManagerComponent))
	{
		UE_LOG(LogGalaxy, Error, TEXT("%s was probed for traversal by %s to %s, but the connection manager component DNE?"),
			*GetName(),
			IsValid(ReservingShip) ? *ReservingShip->GetName() : TEXT("(INVALID SHIP)"),
			IsValid(TargetNode) ? *TargetNode->GetName() : TEXT("(INVALID NODE)"))
	}

	return ConnectionManagerComponent->ReserveShipTraversal(TargetNode, ReservingShip);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Query/Write Functions /////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * A ship has moved here, conquered here, been built here, etc.
 * Physically move it and claim it.
******************************************************************************/
void ADMGalaxyNode::SetCurrentShip(ADMShip* NewShip)
{
	if (!HasAuthority())
	{
		UE_LOG(LogGalaxy, Error, TEXT("ADMGalaxyNode::SetCurrentShip: %s tried to set its current ship to %s, but not on the main server?"),
			*GetName(),
			IsValid(NewShip) ? *NewShip->GetName() : TEXT("INVALID SHIP"))
		return;
	}

	if (!IsValid(NewShip))
	{
		UE_LOG(LogGalaxy, Warning, TEXT("ADMGalaxyNode::SetCurrentShip: %s tried to set its current ship to an invalid ship; Did you mean to use ADMGalaxyNode::RemoveShip?"),
			*GetName());
			
		// In theory we could call RemoveShip here, but its better for designers to fix the error themselves rather then push bad "working" code
		return;
	}

	// remove it from its current node
	if (ADMGalaxyNode* pParent = Cast<ADMGalaxyNode>(NewShip->GetCurrentNode()))
	{
		pParent->RemoveShip();
	}

	// Get the gamemode
	UWorld* pWorld = GetWorld();
	AGameModeBase* pGameModeBase = IsValid(pWorld) ? pWorld->GetAuthGameMode() : nullptr;
	ADMGameMode* pGameMode = Cast<ADMGameMode>(pGameModeBase);
	
	// set the ships position
	FVector ShipPosition = GetActorLocation();
	ShipPosition.Z += pGameMode->GetShipSpawnZOffset();
	NewShip->SetActorLocation(ShipPosition);
	if (!NewShip->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform))
	{
		UE_LOG(LogGalaxy, Error, TEXT("ADMGalaxyNode::SetCurrentShip: %s tried to set its current ship to %s, but the attachment failed"),
			*GetName(),
			*NewShip->GetName())
		return;
	}

	// (TF2 Heavy voice) OURS NOW
	CurrentShip = NewShip;

}

/******************************************************************************
 * Calculate the power of all factions attack this node
******************************************************************************/
void ADMGalaxyNode::GetPendingPowers(TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>>& Powers)
{
	// Account for the current ship on the planet (if there is one)
	if (IsValid(CurrentShip))
	{
		Powers.Add(CurrentShip->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(CurrentShip, 1));
	}

	// Process all pending ships
	for (auto AttemptedShip : PendingShips)
	{
		ADMShip* pShipPtr = AttemptedShip.Key;
		bool Supporting = AttemptedShip.Value;

		TPair<ADMShip*, size_t>* CurrentAttacking = Powers.Find(pShipPtr->TeamComponent->GetTeam());

		// Attacker: No previous attacker -> Add the team to the map
		// Supporter: No previous attacker -> Add the team to the map
		// Attacker: Yes previous attacker -> Ignore
		// Supporter: Yes previous attacker -> increment power
		if (CurrentAttacking == nullptr)
		{
			// this could be one line but it would be really annoying to read
			if (Supporting)
			{
				Powers.Add(pShipPtr->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(nullptr, pShipPtr->GetShipPower()));
			}
			else
			{
				Powers.Add(pShipPtr->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(pShipPtr, pShipPtr->GetShipPower()));
			}
		}
		else
		{
			if (Supporting)
			{
				++CurrentAttacking->Value;
			}
			else if (CurrentAttacking->Key != nullptr)
			{
				CurrentAttacking->Key = pShipPtr;
				++CurrentAttacking->Value;
			}
			// Just ignore the ship if there are multiple attackers
			// TMDOTO: Imagine a situation:
			// Planet A (Team1) has a ship of power 1
			// Planet B (Team1) has a ship of power 1
			// Planet C (Team2)has a ship of power 2
			// 
			// C Is attacking A
			// A's ship wants to move to some planet D, but doesn't know if it will bounce
			// if A bounces and B Supports A, A defends successfully
			// if A bounces and B Moves to A, A fails defense (B cannot move to A, power not counted)
			// Note; it's not like team 1 will KNOW C is attacking A, so they wont know; move or support?
		}
	}
}
