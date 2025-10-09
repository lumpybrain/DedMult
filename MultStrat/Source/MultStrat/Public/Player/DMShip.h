// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMShip.generated.h"

enum class EDMPlayerTeam : uint8;
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
	ADMShip(const FObjectInitializer& ObjectInitializer);

	/** Replication */
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Team that owns the ship and can issue it commands */
	UPROPERTY(BlueprintReadOnly, Replicated)
	EDMPlayerTeam Team;

	/** Player that owns the ship for debug/possibility of alliances in the future */
	UPROPERTY(BlueprintReadWrite, Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer;
	
};
