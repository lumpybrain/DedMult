// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMGalaxyNode.h"

#include "Commands/DMCommand.h"						// UDMCommand
#include "Commands/DMCommand_MoveShip.h"			// UDMCommand_MoveShip
#include "Commands/DMCommandQueueSubsystem.h"		// LogCommands
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

	DOREPLIFETIME(ADMGalaxyNode, pCurrentShip);
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
	ADMShip* pCurrShip = GetShip();
	if (pCurrShip == nullptr || !pCurrShip->CommandsComponent->CheckForCommandFlags(ECommandFlags::MovingShip))
	{
		return true;
	}

	// 3; current ship Does not matter for result (win or tie for home team without it)  = resolvable
	// Map of all teams trying to take control of the planet; mapping their team to the main attacking ship and the total power of their fleet
	TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>> Powers;
	EDMPlayerTeam WinningTeam = GetPendingPowers(Powers);
	if (WinningTeam != TeamComponent->GetTeam())
	{
		// we're not winning this node with or without the ship; we can process
		return true;
	}

	// Find the power diff
	size_t HighestPower = 0;
	size_t PowerDiff = 0;
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
		}
		else if (Power == HighestPower)
		{
			// No winner in the case of a tie
			PowerDiff = 0;
		}
	}
	if (WinningTeam == TeamComponent->GetTeam() &&
		PowerDiff >= pCurrShip->GetShipPower())
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
	EDMPlayerTeam eWinner = GetPendingPowers(Powers);


	FString NodeTeam = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)TeamComponent->GetTeam());
	UE_LOG(LogGalaxy, Display, TEXT("%s combat results (Previous Owner: %s): "),
		*GetName(),
		*NodeTeam)

	// Print Debug (and find the winning ship for later
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
		if (eWinner == eTeam)
		{
			WinningShip = pShipPtr;
			AttackDebug.Append(" and won the battle!");
		}

		UE_LOG(LogGalaxy, Display, TEXT("%s"), *AttackDebug)
	}

	// Declare the winner!
	if (eWinner != EDMPlayerTeam::Unowned)
	{
		if (IsValid(pCurrentShip) && pCurrentShip != WinningShip)
		{
			// DMTODO: Ship Retreats
			pCurrentShip->Destroy();
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
 * A ship on this planet is allowed to move or support if and only if
 * 1: we're not trying to move to a node where we already have an unmoving ship
 * 2: this ship is moving to a node which is not involved in a attack
 *		that is strong then the node's defenses
******************************************************************************/
void ADMGalaxyNode::PreresolveMovingShips()
{
	// no ship? resolved
	if (!IsValid(pCurrentShip))
	{
		return;
	}

	// the ship isn't moving? resolved
	UDMCommand* pOurMove = pCurrentShip->CommandsComponent->GetCommand(UDMCommand_MoveShip::StaticClass());
	if (pOurMove == nullptr)
	{
		return;
	}

	ADMGalaxyNode* pTargetNode = pOurMove->GetTargetNode();
	check(pTargetNode);
	if (!IsValid(pTargetNode))
	{
		UE_LOG(LogCommands, Error, TEXT("Node %s has a ship on it with a moving command, but that moving command does not have a valid target?"),
			*GetName())
		return;
	}

	// the target doesn't have a ship? we can move. resolved.
	const ADMShip* pOpposingShip = pTargetNode->GetShip();
	if (pOpposingShip == nullptr)
	{
		return;
	}

	// ... we're trying to move to a node we control whose ship is not moving? Cancel invalid move
	if (pOpposingShip->TeamComponent->IsSameTeam(TeamComponent) &&
		!pOpposingShip->CommandsComponent->CheckForCommandFlags(ECommandFlags::MovingShip))
	{
		pCurrentShip->CommandsComponent->RemoveCommandFlags(ECommandFlags::MovingShip);
		pCurrentShip->CommandsComponent->UnregisterCommand(pOurMove);
		check(pTargetNode->RemovePendingShip(pCurrentShip));
		return;
	}

	// they aren't targetting us? Resolved
	UDMCommand* pTheirMove = pOpposingShip->CommandsComponent->GetCommand(UDMCommand_MoveShip::StaticClass());
	if (pTheirMove == nullptr || pTheirMove->GetTargetNode() != this)
	{
		return;
	}

	// The target is attacking with greater than or equal to our defending power?
	// cancel our move (cancel their move too if its a tie)
	TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>> Powers;
	EDMPlayerTeam eWinner = GetPendingPowers(Powers);

	TPair<ADMShip*, size_t>* OurPower = Powers.Find(pCurrentShip->TeamComponent->GetTeam());
	check(OurPower);
	TPair<ADMShip*, size_t>* TheirPower = Powers.Find(pOpposingShip->TeamComponent->GetTeam());
	check(TheirPower);

	if (OurPower->Value <= TheirPower->Value)
	{
		pCurrentShip->CommandsComponent->RemoveCommandFlags(ECommandFlags::MovingShip);
		pCurrentShip->CommandsComponent->UnregisterCommand(pOurMove);
		check(pTargetNode->RemovePendingShip(pCurrentShip));
	}
	if (OurPower->Value == TheirPower->Value)
	{
		pOpposingShip->CommandsComponent->RemoveCommandFlags(ECommandFlags::MovingShip);
		pOpposingShip->CommandsComponent->UnregisterCommand(pTheirMove);
		check(RemovePendingShip(pOpposingShip));
	}
}

/******************************************************************************
 * Used by commands to remove the current ship
 * Can also be called by planet code (i.e during collapse) to free the ship 
 *		from its grip
******************************************************************************/
void ADMGalaxyNode::RemoveShip(const UDMCommand* OwningCommand)
{
	// Remove the current ship, being mindful if its technically in the middle of destruction/garbage collection
	if (!IsValid(pCurrentShip))
	{
		UE_LOG(LogGalaxy, Warning, TEXT("%s: \"%s\" tried to remove the current ship, Current Ship is invalid."),
			*GetName(),
			IsValid(OwningCommand) ? *OwningCommand->CommandDebug() : TEXT("Manual Code Call"))
	}
	else
	{
		pCurrentShip->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	pCurrentShip = nullptr;
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
bool ADMGalaxyNode::RemovePendingShip(const ADMShip* OldShip)
{
	return PendingShips.Remove(OldShip) != 0;
}

/******************************************************************************
 * Get all the ships currently trying to move onto this planet.
 * Does not include supporters.
 * Note, this is ONLY valid while commands are running, and should NOT 
 *		be queried outside of the command run loop.
 *
 * Returns an array of all ships trying to move to the node
******************************************************************************/
void ADMGalaxyNode::GetPendingAttackers(TArray<ADMShip*>& OutShips)
{
	for (auto AttemptedShip : PendingShips)
	{
		if (IsValid(AttemptedShip.Key))
		{
			OutShips.Add(AttemptedShip.Key);
		}
	}
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
	
	// set the ships position (and account for scale)
	if (!NewShip->AttachToActor(this, FAttachmentTransformRules::SnapToTargetNotIncludingScale))
	{
		UE_LOG(LogGalaxy, Error, TEXT("ADMGalaxyNode::SetCurrentShip: %s tried to set its current ship to %s, but the attachment failed"),
			*GetName(),
			*NewShip->GetName())
		return;
	}
	float RelativeZ = (1.0f / GetTransform().GetScale3D().Z)* pGameMode->GetShipSpawnZOffset();
	NewShip->SetActorRelativeLocation(FVector(0, 0, RelativeZ));

	// (TF2 Heavy voice) OURS NOW
	pCurrentShip = NewShip;

}

/******************************************************************************
 * Calculate the power of all factions attack this node
 * 
 * Returns the team with the highest power level; Unowned if there's a tie
******************************************************************************/
EDMPlayerTeam ADMGalaxyNode::GetPendingPowers(TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>>& Powers)
{
	// Account for the current ship on the planet (if there is one)
	if (IsValid(pCurrentShip))
	{
		Powers.Add(pCurrentShip->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(pCurrentShip, 1));
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
			else if (CurrentAttacking->Key == nullptr)
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

	EDMPlayerTeam StrongestTeam = EDMPlayerTeam::Unowned;
	size_t HighestPower = 0;
	for (auto iter : Powers)
	{
		// if the teams pwoer level is the highest we've seen
		// and the team has a valid attacking ship
		if (iter.Value.Value > HighestPower && iter.Value.Key != nullptr)
		{
			HighestPower = iter.Value.Value;
			StrongestTeam = iter.Key;
		}
		else if (iter.Value.Value == HighestPower)
		{
			StrongestTeam = EDMPlayerTeam::Unowned;
		}
	}

	return StrongestTeam;
}
