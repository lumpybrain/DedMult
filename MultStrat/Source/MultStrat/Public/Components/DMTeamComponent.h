// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMTeamComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeams, Log, All);

UENUM(BlueprintType)
enum class EDMPlayerTeam : uint8
{
	Invalid = 0,
	Unowned,
	TeamOne,
	TeamTwo,
	TeamThree,
	TeamFour,
	TeamFive,
	TeamSix,
	TeamSeven,
	TeamEight,
	Count
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorTeamChanged, AActor*, ChangedActor, EDMPlayerTeam, NewTeam);

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class MULTSTRAT_API UDMTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	/** Turn off tick in constructor */
	UDMTeamComponent();

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~=============================================================================
	// Team Testing

	/** Event for when Owning Team has changed. */
	UPROPERTY(BlueprintAssignable)
	FActorTeamChanged OnActiveTeamChanged;

	/**
	 * Test if two actors are on the same team.
	 * if one of the actors does not have a Team component, this will always fail.
	 */
	UFUNCTION(BlueprintCallable)
	static bool ActorsAreSameTeam(const AActor* FirstActor, const AActor* SecondActor);

	/**
	 * Test if some actor is on the same team as this component
	 * If the actor does not have a Team component, this will always fail.
	 */
	UFUNCTION(BlueprintCallable)
	bool ActorIsSameTeam(const AActor* OtherActor) const;

	/** Test if two team components are on the same team */
	UFUNCTION(BlueprintCallable)
	bool IsSameTeam(const UDMTeamComponent* OtherComponent) const;

	/**
	 * Test if some actor is on the same team as this component
	 * If the actor does not have a Team component, this will always fail.
	 */
	UFUNCTION(BlueprintCallable)
	static EDMPlayerTeam GetActorsTeam(const AActor* TargetActor);

	/** Get the components active team*/
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam GetTeam() const															{ return ActiveTeam; }

	/** 
	 * Settor for the components team. Stores the active team in PreviousTeam 
	 * returns the new team for easy chaining
	 */
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam SetTeam(EDMPlayerTeam NewTeam);

	/** Get the components previous team */
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam GetPreviousTeam() const													{ return PreviousTeam; }

protected:
	/** Broadcast an event when the team changes */
	UFUNCTION()
	void OnRep_ActiveTeam();
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ActiveTeam)
	EDMPlayerTeam ActiveTeam;

	UPROPERTY(Replicated)
	EDMPlayerTeam PreviousTeam;
};
