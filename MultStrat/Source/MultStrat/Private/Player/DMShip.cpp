// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMShip.h"

#include "Net/UnrealNetwork.h"	// DOREPLIFETIME


/******************************************************************************
 * Constructor: enable replication
******************************************************************************/
ADMShip::ADMShip(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
}

/******************************************************************************
 * Replication
******************************************************************************/
void ADMShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMShip, Team);
	DOREPLIFETIME(ADMShip, OwningPlayer);

}