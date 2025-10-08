// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMGalaxyNode.generated.h"

class ADMGalaxyNode;
class ADMShip;
class UDMCommand;

DECLARE_LOG_CATEGORY_EXTERN(LogGalaxy, Log, All);

/**
 * 
 */
UCLASS()
class MULTSTRAT_API UDMNodeConnectionManager : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<ADMGalaxyNode*>	ConnectedNodes;
};

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMGalaxyNode : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ADMGalaxyNode(const FObjectInitializer& ObjectInitializer);
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Turn Functions
	void ResolveTurn();

	// Query/Write Functions
	UFUNCTION(BlueprintCallable)
	bool K2_HasShip() const				{ return CurrentShip != nullptr; }
	UFUNCTION(BlueprintCallable)
	bool K2_HasIncomingShip(FString& OutOwningCommandDebug) const;
	bool HasIncomingShip() const		{ return IncomingShip; }

	bool SetIncomingShip(bool Incoming, UDMCommand* OwningCommand);
	virtual void RemoveShip(UDMCommand* OwningCommand);
	bool AddPendingShip(ADMShip* NewShip, bool IsSupportingTeam, UDMCommand* OwningCommand);


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDMNodeConnectionManager> ConnectionManagerComponent;
	
protected:

	virtual void SetCurrentShip(ADMShip* NewShip);

	// NOTE: These variables are NOT replicated because they are used to validate commands based on information the client has

	// Current ship docked at this node
	UPROPERTY(Replicated)
	TObjectPtr<ADMShip> CurrentShip = nullptr;

	TSet<TPair<TObjectPtr<ADMShip>, bool>> PendingShips;

	// A ship wants to come to this node (i.e, build or move command is in the queue)
	bool IncomingShip = false;
	TObjectPtr<UDMCommand> IncomingShipCommand = nullptr;
};
