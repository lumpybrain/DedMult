// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMShip.generated.h"

class ADMPlayerState;

/**
 * Base class for ships players will use to conquer or collect resources
 */
UCLASS()
class MULTSTRAT_API ADMShip : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	/** Constructor: enable replication */
	ADMShip(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	ADMPlayerState* K2_GetOwningPlayer()			{ return OwningPlayer; }
	TObjectPtr<ADMPlayerState> GetOwningPlayer()	{ return OwningPlayer; }

	/** Team that owns the ship and can issue it commands */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDMTeamComponent> TeamComponent;

protected:

	/** Player that owns the ship for debug/possibility of alliances in the future */
	UPROPERTY(BlueprintReadWrite, Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer;
	
};
