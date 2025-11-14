// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMPlanetProcessingSubsystem.h"

#include "Components\DMNodeConnectionComponent.h"	// ADMConnector
#include "Kismet/GameplayStatics.h"					// UGameplayStatics
#include "GalaxyObjects/DMGalaxyNode.h"				// ADMGalaxyNode
#include "GameSettings/DMGameState.h"				// ADMGameState
#include "Player/DMShip.h"							// ADMShip
#include "Components/DMCommandFlagsComponent.h"		// UDMCommandComponent


/******************************************************************************
 * Constructor
******************************************************************************/
UDMPlanetProcessingSubsystem::UDMPlanetProcessingSubsystem() : Super()
{
	SetTickableTickType(ETickableTickType::Never);
}
/******************************************************************************
 * Static Gettor
******************************************************************************/
UDMPlanetProcessingSubsystem* UDMPlanetProcessingSubsystem::Get(UObject* WorldContextObject)
{
	UWorld* pWorld = WorldContextObject != nullptr ? WorldContextObject->GetWorld() : nullptr;
	UDMPlanetProcessingSubsystem* pQueueSubsystem = pWorld != nullptr ? pWorld->GetSubsystem<UDMPlanetProcessingSubsystem>() : nullptr;

	return pQueueSubsystem;
}

/******************************************************************************
 * UWorldSubsystem Override
 * 
 * Begin Play: Disable Tick
******************************************************************************/
void UDMPlanetProcessingSubsystem::OnWorldBeginPlay(UWorld& InWorld) /*override*/
{
	SetTickableTickType(ETickableTickType::Never);
}

/******************************************************************************
 * UTickableWorldSubsystem Override
 *
 * If we're ticking, we should be processing player combat/ship movement
******************************************************************************/
void UDMPlanetProcessingSubsystem::Tick(float DeltaTime) /*override*/
{
	switch (CurrentStage)
	{
	case EProcessingStage::MoveShips:
		MovePendingShipsToPlanets();
		break;
	case EProcessingStage::Combat:
		ProcessPlanetCombat();
		break;
	default:
		break;
	}
}

/******************************************************************************
 * FTickableGameObject Override
 *
 * Need a Stat ID to track the performance of this object 
******************************************************************************/
TStatId UDMPlanetProcessingSubsystem::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(UDMPlanetProcessingSubsystem, STATGROUP_Tickables);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Planet Processing Functions ///////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/*
 * Register a "floating ship" that has been detatched from its home planet
 * without a new planet to call home yet
 */
void UDMPlanetProcessingSubsystem::RegisterFloatingShip(ADMShip* FloatingShip)
{
	AllFloatingShips.Add(FloatingShip);
}

/******************************************************************************
 * Called when the subsystem should start moving/animating planets
******************************************************************************/
void UDMPlanetProcessingSubsystem::StartProcessingPlanetResults()
{
	CurrentStage = EProcessingStage::MoveShips;
	SetTickableTickType(ETickableTickType::Always);


	TArray<AActor*> AllBasicObjects;
	UGameplayStatics::GetAllActorsOfClass(this, ADMBaseGalaxyObject::StaticClass(), AllBasicObjects);
	for (AActor* pBaseButActor : AllBasicObjects)
	{
		// broadcast changes we're going to make to al clients
		pBaseButActor->SetNetDormancy(ENetDormancy::DORM_Never);

		if (ADMGalaxyNode* pNode = Cast<ADMGalaxyNode>(pBaseButActor))
		{
			pNode->PreresolveMovingShips();

			AllGalaxyNodes.Add(pNode);
		}
	}

	// DMTODO: This should be all dirty galaxynodes, that register themselves when commands run
	TArray<AActor*> AllNodesAsActors;
	UGameplayStatics::GetAllActorsOfClass(this, ADMGalaxyNode::StaticClass(), AllNodesAsActors);
	for (AActor* pNodeAsActor : AllNodesAsActors)
	{
		ADMGalaxyNode* pNode = Cast<ADMGalaxyNode>(pNodeAsActor);
		pNode->PreresolveMovingShips();

		AllGalaxyNodes.Add(pNode);
	}
}

/******************************************************************************
 * Let the planets start moving their respective ships to them
******************************************************************************/
void UDMPlanetProcessingSubsystem::MovePendingShipsToPlanets()
{
	// DMTODO: Slide all the ships to their current planets/wait for animations of all the ships moving to finish
	CurrentStage = EProcessingStage::Combat;
}

/******************************************************************************
 * Let the planets resolve their combat
******************************************************************************/
void UDMPlanetProcessingSubsystem::ProcessPlanetCombat()
{
	// keep track as to whether we're going to finish resolving all nodes this cycle
	bool NodeProcessingFinished = true;

	// Find all resolveable nodes
	TArray<ADMGalaxyNode*> ResolveableNodes;
	for (ADMGalaxyNode* pNode : AllGalaxyNodes)
	{
		if (!pNode->CommandsComponent->CheckForCommandFlags(ECommandFlags::Resolved))
		{
			if (pNode->CanResolveTurn())
			{
				ResolveableNodes.Add(pNode);
				pNode->CommandsComponent->AddCommandFlags(ECommandFlags::Resolved);
			}
			else
			{
				NodeProcessingFinished = false;
			}
		}
	}

	// if no nodes are resolveable, resolve all remaining unresolved nodes
	if (ResolveableNodes.IsEmpty())
	{
		NodeProcessingFinished = true;
		for (ADMGalaxyNode* pNode : AllGalaxyNodes)
		{
			if (!pNode->CommandsComponent->CheckForCommandFlags(ECommandFlags::Resolved))
			{
				ResolveableNodes.Add(pNode);
				pNode->CommandsComponent->AddCommandFlags(ECommandFlags::Resolved);
			}
		}
	}
	
	// Resolve nodes
	for (ADMGalaxyNode* pNode : ResolveableNodes)
	{
		pNode->ResolveTurn();
	}

	// if all nodes resolved, move to next step
	if (NodeProcessingFinished)
	{
		for (ADMGalaxyNode* pNode : AllGalaxyNodes)
		{
			pNode->CommandsComponent->RemoveCommandFlags(ECommandFlags::Resolved);
		}
		ProcessingFinished();
	}

}

/******************************************************************************
 * Clean up. Tell the game state we're all done processing.
******************************************************************************/
void UDMPlanetProcessingSubsystem::ProcessingFinished()
{
	// Tell the gamestate we're done
	ADMGameState* pState = ADMGameState::Get(this);
	ensure(pState);
	if (pState != nullptr)
	{
		pState->TurnProcessingFinished();
	}

	TArray<AActor*> AllBasicObjects;
	UGameplayStatics::GetAllActorsOfClass(this, ADMBaseGalaxyObject::StaticClass(), AllBasicObjects);
	for (AActor* pBaseButActor : AllBasicObjects)
	{
		// Let clients go nuts again
		pBaseButActor->SetNetDormancy(ENetDormancy::DORM_DormantAll);

		ADMBaseGalaxyObject* pBase = Cast<ADMBaseGalaxyObject>(pBaseButActor);
		pBase->CommandsComponent->Reset();
	}

	for (ADMShip* pShip : AllFloatingShips)
	{
		if (pShip->GetParentActor() == nullptr)
		{
			pShip->Destroy();
		}
	}

	// cleanup
	AllGalaxyNodes.Empty();
	AllFloatingShips.Empty();
	SetTickableTickType(ETickableTickType::Never);
}
