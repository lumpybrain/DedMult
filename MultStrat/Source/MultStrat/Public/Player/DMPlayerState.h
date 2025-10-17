// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DMPlayerState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerTurnProcessed);

/**
 * Stores information on the current state of the player's turn submission and team
 */
UCLASS()
class MULTSTRAT_API ADMPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	/** Constructor */
	ADMPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Get whether the controller submitted its turn */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool GetTurnSubmitted() const				{ return bTurnSubmitted; }
	void SetTurnSubmitted(bool IsSubmitted)		{ bTurnSubmitted = IsSubmitted; }

	UFUNCTION(Client, Reliable)
	void TurnProcessed();
	void TurnProcessed_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TObjectPtr<class UDMTeamComponent> TeamComponent;

	/** Event for when Owning Team has changed. */
	UPROPERTY(BlueprintAssignable)
	FPlayerTurnProcessed OnTurnProcessed;


protected:
	/** Trigger this boolean when we've submitted or cancelled our turn locally */
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bTurnSubmitted;
};
