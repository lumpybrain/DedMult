// Copyright (c) 2025 William Pritz under MIT License


#include "Components/DMTeamComponent.h"

#include "Net/UnrealNetwork.h"			// DOREPLIFETIME



DEFINE_LOG_CATEGORY(LogTeams);


/******************************************************************************
 * UDMTeamComponent Constructor: Don't Tick!
******************************************************************************/
UDMTeamComponent::UDMTeamComponent()
{
	// don't tick!
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

/******************************************************************************
 * Replication
******************************************************************************/
void UDMTeamComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const /* override */
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDMTeamComponent, ActiveTeam);
	DOREPLIFETIME(UDMTeamComponent, PreviousTeam);
}

/*/////////////////////////////////////////////////////////////////////////////
*	Team Testing //////////////////////////////////////////////////////////////
*//////////////////////////////////////////////////////////////////////////////

/******************************************************************************
 * Test if two actors are on the same team.
 * if one of the actors does not have a Team component, this will always fail.
******************************************************************************/
bool UDMTeamComponent::ActorsAreSameTeam(const AActor* FirstActor, const AActor* SecondActor)
{
	if (!IsValid(FirstActor))
	{
		return false;
	}
	const UDMTeamComponent* FirstComponent = FirstActor->GetComponentByClass<UDMTeamComponent>();
	if (!IsValid(FirstComponent))
	{
		return false;
	}
	
	return FirstComponent->ActorIsSameTeam(SecondActor);
}

/******************************************************************************
 * Test if some actor is on the same team as this component
 * If the actor does not have a Team component, this will always fail.
******************************************************************************/
bool UDMTeamComponent::ActorIsSameTeam(const AActor* OtherActor) const
{
	if (!IsValid(OtherActor))
	{
		return false;
	}

	return IsSameTeam(OtherActor->GetComponentByClass<UDMTeamComponent>());
}

/******************************************************************************
 * Test if two team components are on the same team
******************************************************************************/
bool UDMTeamComponent::IsSameTeam(const UDMTeamComponent* OtherComponent) const
{
	return IsValid(OtherComponent) ? ActiveTeam == OtherComponent->ActiveTeam : false;
}

/******************************************************************************
 * Test if some actor is on the same team as this component
 * If the actor does not have a Team component, returns EDMPlayerTeam::Invalid
******************************************************************************/
EDMPlayerTeam UDMTeamComponent::GetActorsTeam(const AActor* TargetActor)
{
	if (!IsValid(TargetActor))
	{
		return EDMPlayerTeam::Invalid;
	}

	const UDMTeamComponent* TargetComponent = TargetActor->GetComponentByClass<UDMTeamComponent>();
	if (!IsValid(TargetComponent))
	{
		return EDMPlayerTeam::Invalid;
	}

	return TargetComponent->ActiveTeam;
}

/******************************************************************************
 * Settor for the components team. Stores the active team in PreviousTeam.
 * returns the new team for easy chaining
******************************************************************************/
EDMPlayerTeam UDMTeamComponent::SetTeam(EDMPlayerTeam NewTeam)
{
	// print debug
	AActor* pOwner = GetOwner();
	FString EnumName = StaticEnum<EDMPlayerTeam>()->GetAuthoredNameStringByIndex((int32)NewTeam);
	UE_LOG(LogTeams, Display, TEXT("%s set to team %s"),
		IsValid(pOwner) ? *pOwner->GetName() : TEXT("INVALID OWNER"),
		*EnumName)

	// swap
	PreviousTeam = ActiveTeam;
	ActiveTeam = NewTeam;

	return ActiveTeam;
}

/******************************************************************************
 * Broadcast an event when the team changes
******************************************************************************/
void UDMTeamComponent::OnRep_ActiveTeam()
{
	OnActiveTeamChanged.Broadcast(GetOwner(), ActiveTeam);
}