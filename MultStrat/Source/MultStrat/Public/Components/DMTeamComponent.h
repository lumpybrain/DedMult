// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DMTeamComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeams, Log, All);

UENUM()
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
	static bool K2_ActorsAreSameTeam(const AActor* FirstActor, const AActor* SecondActor)	{ return ActorsAreSameTeam(FirstActor, SecondActor); }
	static bool ActorsAreSameTeam(const AActor* FirstActor, const AActor* SecondActor);

	/**
	 * Test if some actor is on the same team as this component
	 * If the actor does not have a Team component, this will always fail.
	 */
	UFUNCTION(BlueprintCallable)
	bool K2_ActorIsSameTeam(const AActor* OtherActor) const									{ return ActorIsSameTeam(OtherActor); }
	bool ActorIsSameTeam(const AActor* OtherActor) const;

	/** Test if two team components are on the same team */
	UFUNCTION(BlueprintCallable)
	bool K2_IsSameTeam(const UDMTeamComponent* OtherComponent) const						{ return IsSameTeam(OtherComponent); }
	bool IsSameTeam(const UDMTeamComponent* OtherComponent) const;

	/**
	 * Test if some actor is on the same team as this component
	 * If the actor does not have a Team component, this will always fail.
	 */
	UFUNCTION(BlueprintCallable)
	static EDMPlayerTeam K2_GetActorsTeam(const AActor* TargetActor)						{ return UDMTeamComponent::GetActorsTeam(TargetActor); }
	static EDMPlayerTeam GetActorsTeam(const AActor* TargetActor);

	/** Get the components active team*/
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam K2_GetTeam() const														{ return ActiveTeam; }
	EDMPlayerTeam GetTeam() const															{ return ActiveTeam; }

	/** 
	 * Settor for the components team. Stores the active team in PreviousTeam 
	 * returns the new team for easy chaining
	 */
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam K2_SetTeam(EDMPlayerTeam NewTeam)											{ return SetTeam(NewTeam); }
	EDMPlayerTeam SetTeam(EDMPlayerTeam NewTeam);

	/** Get the components previous team */
	UFUNCTION(BlueprintCallable)
	EDMPlayerTeam K2_GetPreviousTeam() const												{ return PreviousTeam; }
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
