// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMShip.h"

#include "Components/DMTeamComponent.h"		// UDMTeamComponent
#include "GalaxyObjects/DMGalaxyNode.h"		// ADMGalaxyNode, UDMNodeConnectionManager
#include "Net/UnrealNetwork.h"				// DOREPLIFETIME


/******************************************************************************
 * Constructor: enable replication
******************************************************************************/
ADMShip::ADMShip(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;

	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
}

/******************************************************************************
 * Replication
******************************************************************************/
void ADMShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMShip, OwningPlayer);

}

/******************************************************************************
 * Get the current galaxy node the ship is docked at.
 * Note: May be nullptr if commands are currently executing.
******************************************************************************/
ADMGalaxyNode* ADMShip::GetCurrentNode() const
{
	return Cast<ADMGalaxyNode>(GetAttachParentActor());
}


/******************************************************************************
 * Returns true if a target node is reachable in one movement command
 * Note: Returns false if the target node is the ships current node.
******************************************************************************/
bool ADMShip::IsNodeReachable_Implementation(const ADMGalaxyNode* pTargetNode) const
{
	// Get the current node that we are docked at
	// This function should be unreachable when the parent is null (while commands are running)
	const ADMGalaxyNode* pCurrentNode = Cast<ADMGalaxyNode>(GetAttachParentActor());
	check(pCurrentNode);

	if (pCurrentNode == pTargetNode)
	{
		return false;
	}


	// Check all connections to our current node for a match
	const UDMNodeConnectionManager* pConnectManager = pCurrentNode->GetConnectionManager();
	check(pConnectManager);
	for (const ADMGalaxyNode* pCurrNode : pConnectManager->ConnectedNodes)
	{
		if (pCurrNode == pTargetNode)
		{
			return true;
		}
	}

	return false;
}