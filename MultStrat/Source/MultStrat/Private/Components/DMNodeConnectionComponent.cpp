// Copyright (c) 2025 William Pritz under MIT License


#include "Components/DMNodeConnectionComponent.h"

#include "Components/SplineComponent.h"			// USplineComponent
#include "GalaxyObjects/DMGalaxyNode.h"			// LogGalaxy, ADMGalaxyNode
#include "Net/UnrealNetwork.h"					// DOREPLIFETIME
#include "Player/DMShip.h"						// ADMShip
#include "Components/DMCommandFlagsComponent.h"	// UDMActiveCommandsComponent, ECommandFlags

/*/////////////////////////////////////////////////////////////////////////////
*	UDMNodeConnectionComponent ////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Constructor: Don't Tick!
******************************************************************************/
UDMNodeConnectionComponent::UDMNodeConnectionComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

/******************************************************************************
 * Replication
******************************************************************************/
void UDMNodeConnectionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDMNodeConnectionComponent, ConnectedNodes);
}

/******************************************************************************
 * On BeginPlay, Construct or find our connectors
******************************************************************************/
void UDMNodeConnectionComponent::BeginPlay() /* override */
{
	Super::BeginPlay();

	ConnectorSplines.Init(nullptr, ConnectedNodes.Num());

	AActor* pOwner = GetOwner();
	ADMGalaxyNode* pNodeOwner = Cast<ADMGalaxyNode>(pOwner);
	check(pNodeOwner);

	if (!IsValid(pNodeOwner))
	{
		UE_LOG(LogGalaxy, Error, TEXT("UDMNodeConnectionComponent is on %s, which is not a GalaxyNode. UDMNodeConnectionComponent is not built for this class!"),
			*pOwner->GetName())
	}

	// Either find or create a new connector for each connected node
	for (size_t i = 0; i < ConnectedNodes.Num(); ++i)
	{
		UDMNodeConnectionComponent* pOtherConnector = ConnectedNodes[i]->GetConnectionManager();
		check(pOtherConnector);

		if (ADMConnector* pOtherNodesConnector = pOtherConnector->GetConnectorForNode(pNodeOwner))
		{
			ConnectorSplines[i] = pOtherNodesConnector;
		}
		else
		{
			if (ConnectorClass == nullptr)
			{
				UE_LOG(LogGalaxy, Error, TEXT("Class %s does not have a valid Connector Class set; no connectors will be made for this node!"),
					*pOwner->GetClass()->GetName())

				return;
			}

			UWorld* pWorld = GetWorld();
			check(pWorld);
			ADMConnector* pNewConnector = pWorld->SpawnActor<ADMConnector>(ConnectorClass);
			pNewConnector->InitializeSplineMesh(pNodeOwner, ConnectedNodes[i]);
			ConnectorSplines[i] = pNewConnector;
		}

	}
}

/******************************************************************************
 * Reserve a connector from this planet to another planet
 * If this function is called while another ship has reserved the conector,
 *		we will "bounce" that ship's movement
 * 
 * Returns true if the spot is reserved; returns false if the connector is in
 *		use, or if the connection DNE
******************************************************************************/
bool UDMNodeConnectionComponent::ReserveShipTraversal(ADMGalaxyNode* pTargetNode, ADMShip* pReservingShip)
{
	if (!IsValid(pReservingShip))
	{
		UE_LOG(LogGalaxy, Error, TEXT("Invalid Ship tried to reserve a flight from node %s to %s"),
			*GetOwner()->GetName(),
			IsValid(pTargetNode) ? *pTargetNode->GetName() : TEXT("(INVALID NODE)"))
		return false;
	}
	if (!IsValid(pTargetNode))
	{
		UE_LOG(LogGalaxy, Error, TEXT("Ship %s tried to reserve a flight from node %s to and invalid node!"),
			*pReservingShip->GetName(),
			*GetOwner()->GetName())
		return false;
	}

	ADMConnector* pConnector = GetConnectorForNode(pTargetNode);
	if (!IsValid(pConnector))
	{
		UE_LOG(LogGalaxy, Error, TEXT("Ship %s tried to reserve a flight from node %s to %s, but that route is not valid!"),
			*pReservingShip->GetName(),
			*GetOwner()->GetName(),
			*pTargetNode->GetName())

			return false;
	}

	ADMShip* pTraversingShip = pConnector->GetTraversingShip();
	if (IsValid(pTraversingShip))
	{
		UE_LOG(LogGalaxy, Display, TEXT("Ships %s and %s both tried to traverse between nodes %s and %s; the ships have bounced"),
			*pTraversingShip->GetName(),
			*pReservingShip->GetName(),
			*GetOwner()->GetName(),
			*pTargetNode->GetName())

		// Tell the original node the bounced ship isn't coming
		ADMGalaxyNode* pOriginalNode = pReservingShip->GetCurrentNode();
		check(pOriginalNode)
		pOriginalNode->RemovePendingShip(pTraversingShip);
		pTraversingShip->CommandsComponent->RemoveCommandFlags(ECommandFlags::MovingShip);

		return false;
	}

	pConnector->SetTraversingShip(pReservingShip);
	return true;
}

/******************************************************************************
 * Find the associated connector for the planet; should be at the same index
 * in the ConnectorSplines array as the planet is in our ConnectedNodes array
******************************************************************************/
ADMConnector* UDMNodeConnectionComponent::GetConnectorForNode(const ADMGalaxyNode* Node)
{
	for (size_t i = 0; i < ConnectedNodes.Num(); ++i)
	{
		if (ConnectorSplines.Num() <= i)
		{
			return nullptr;
		}
		if (ConnectedNodes[i] == Node)
		{
			return ConnectorSplines[i];
		}
	}

	return nullptr;
}
