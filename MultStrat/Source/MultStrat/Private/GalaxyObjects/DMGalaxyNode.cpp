// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMGalaxyNode.h"

#include "Commands/DMCommand.h"						// UDMCommand
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
 * Resolve all pending ships after all the commands have executed for a turn
******************************************************************************/
void ADMGalaxyNode::ResolveTurn()
{
	// Map of all teams trying to take control of the planet; mapping their team to the main attacking ship and the total power of their fleet
	TMap<EDMPlayerTeam, TPair<ADMShip*, size_t>> Powers;

	// Account for the current ship on the planet (if there is one)
	if (IsValid(CurrentShip))
	{
		Powers.Add(CurrentShip->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(CurrentShip, 1));
	}

	// Process all pending ships
	for (TPair<TObjectPtr<ADMShip>, bool> AttemptedShip : PendingShips)
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
				Powers.Add(pShipPtr->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(nullptr, 1));
			}
			else
			{
				Powers.Add(pShipPtr->TeamComponent->GetTeam(), TPair<ADMShip*, size_t>(pShipPtr, 1));
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
			else
			{
				FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)pShipPtr->TeamComponent->GetTeam());
				UE_LOG(LogGalaxy, Warning, TEXT("%s: Multiple attacking ships from team %s."),
					*GetName(),
					*EnumName)
				// Ignore the secondary attacking ship
			}
		}
	}

	// Find the winner
	size_t HighestPower = 0;
	ADMShip* WinningShip = nullptr;
	for (auto TeamPower : Powers)
	{
		EDMPlayerTeam eTeam = TeamPower.Key;
		ADMShip* pShipPtr = TeamPower.Value.Key;
		size_t Power = TeamPower.Value.Value;

		FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)eTeam);
		FString AttackDebug = FString::Printf(TEXT("%s: Attacked by team %s with power %d"),
			*GetName(),
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
		UE_LOG(LogGalaxy, Display, TEXT("%s: Captured by team %s with power %d"),
			*GetName(),
			*WinnerEnumName,
			HighestPower);

		if (IsValid(CurrentShip) && CurrentShip != WinningShip)
		{
			// DMTODO: Ship Retreats
			CurrentShip->Destroy();
		}

		SetCurrentShip(WinningShip);
	}
}

/******************************************************************************
 * Used by a team to register a ship is incoming
 * Only matters to the local player, so they can't do multiple commands to send
 *		a ship to one node
******************************************************************************/
bool ADMGalaxyNode::SetIncomingShip(bool Incoming, const UDMCommand* NewOwningCommand)
{
	if (!IsValid(NewOwningCommand))
	{
		UE_LOG(LogGalaxy, Error, TEXT("%s: Invalid Command tried to set Incoming Ship to %s (Was: %s%s)."),
			*GetName(),
			Incoming ? TEXT("True") : TEXT("False"),
			IncomingShip ? TEXT("True") : TEXT("False"),
			IsValid(IncomingShipCommand) ? *FString::Printf(TEXT(" (%s)"), *IncomingShipCommand->CommandDebug()) : TEXT(""))

			return false;
	}

	if (Incoming)
	{
		if (IncomingShip)
		{
			// It shouldn't be possible for the command to be invalid but the bool to be true; let this crash loud for now
			UE_LOG(LogGalaxy, Error, TEXT("%s: Command \"%s\" tried to own IncomingShip, but it is already owned by command \"%s\"."),
				*GetName(),
				*NewOwningCommand->CommandDebug(),
				*IncomingShipCommand->CommandDebug())

				return false;
		}

		UE_LOG(LogGalaxy, Display, TEXT("%s: Incoming Ship from Command \"%s\"."),
			*GetName(),
			*NewOwningCommand->CommandDebug())
	}
	else
	{
		if (!IncomingShip)
		{
			UE_LOG(LogGalaxy, Warning, TEXT("%s: Command \"%s\" is clearing Incoming Ship, but no ship was incoming."),
				*GetName(),
				*NewOwningCommand->CommandDebug())
		}
		else if (NewOwningCommand != IncomingShipCommand)
		{
			UE_LOG(LogGalaxy, Error, TEXT("%s: Command \"%s\" tried to clear Incoming Ship, but it's owned by Command \"%s\"."),
				*GetName(),
				*NewOwningCommand->CommandDebug(),
				*IncomingShipCommand->CommandDebug())

				return false;
		}
		else
		{
			UE_LOG(LogGalaxy, Display, TEXT("%s: Incoming Ship cleared by Command \"%s\"."),
				*GetName(),
				*NewOwningCommand->CommandDebug())
		}
	}

	IncomingShipCommand = NewOwningCommand;
	IncomingShip = Incoming;
	return true;
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

	PendingShips.Add(TPair<TObjectPtr<ADMShip>, bool>(NewShip, Supporting));
	return true;
}

/*/////////////////////////////////////////////////////////////////////////////
*	Query/Write Functions /////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Gettor with debug output
******************************************************************************/
bool ADMGalaxyNode::K2_HasIncomingShip(FString& OutOwningCommandDebug) const
{
	OutOwningCommandDebug = IsValid(IncomingShipCommand) ? IncomingShipCommand->CommandDebug() : TEXT("None");
	return IncomingShip;
}


UDMNodeConnectionComponent* ADMGalaxyNode::GetConnectionManager() const
{ 
	return ConnectionManagerComponent; 
}

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