// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/DMShip.h"

#include "Net/UnrealNetwork.h"	// DOREPLIFETIME


// ----------------------------------------------------------------------------
ADMShip::ADMShip(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
}

// ----------------------------------------------------------------------------
void ADMShip::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMShip, Team);
	DOREPLIFETIME(ADMShip, OwningPlayer);

}