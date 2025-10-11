// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMShip.h"

#include "Net/UnrealNetwork.h"				// DOREPLIFETIME
#include "Components/DMTeamComponent.h"		// UDMTeamComponent


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