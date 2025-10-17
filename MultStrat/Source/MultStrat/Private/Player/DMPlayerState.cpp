// Copyright (c) 2025 William Pritz under MIT License


#include "Player/DMPlayerState.h"

#include "Components/DMTeamComponent.h"		// UDMTeamComponent
#include "Net/UnrealNetwork.h"				// DOREPLIFETIME

/******************************************************************************
 * Constructor
******************************************************************************/
ADMPlayerState::ADMPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
}

/******************************************************************************
 * Replication
******************************************************************************/
void ADMPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /*override*/
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADMPlayerState, bTurnSubmitted)
}

/******************************************************************************
 * Trigger this boolean when we've submitted or cancelled our turn locally
******************************************************************************/
void ADMPlayerState::TurnProcessed_Implementation()
{
	OnTurnProcessed.Broadcast();
}
