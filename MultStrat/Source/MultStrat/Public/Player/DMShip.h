// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMShip.generated.h"

enum class EDMPlayerTeam : uint8;
class ADMPlayerState;

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMShip : public AStaticMeshActor
{
	GENERATED_BODY()
	
public:
	ADMShip(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Properties
	UPROPERTY(BlueprintReadOnly, Replicated)
	EDMPlayerTeam Team;

	UPROPERTY(BlueprintReadWrite, Replicated)
	TObjectPtr<ADMPlayerState> OwningPlayer;
	
};
