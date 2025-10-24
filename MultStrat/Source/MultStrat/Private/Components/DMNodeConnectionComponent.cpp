// Copyright (c) 2025 William Pritz under MIT License


#include "Components/DMNodeConnectionComponent.h"

#include "Components/SplineComponent.h"	// USplineComponent
#include "GalaxyObjects/DMGalaxyNode.h"	// LogGalaxy, ADMGalaxyNode
#include "Net/UnrealNetwork.h"			// DOREPLIFETIME

/*/////////////////////////////////////////////////////////////////////////////
*	UDMNodeConnectionComponent ////////////////////////////////////////////////////
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
 * Find the associated connector for the planet; should be at the same index
 * in the ConnectorSplines array as the planet is in our ConnectedNodes array
******************************************************************************/
ADMConnector* UDMNodeConnectionComponent::GetConnectorForNode(const ADMGalaxyNode* Node)
{
	for (size_t i = 0; i < ConnectedNodes.Num(); ++i)
	{
		if (ConnectedNodes[i] != Node)
		{
			continue;
		}

		if (ConnectorSplines.Num() <= i)
		{
			return nullptr;
		}

		return ConnectorSplines[i];
	}

	return nullptr;
}
