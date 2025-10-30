// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "DMBaseGalaxyObject.generated.h"

/**
 * 
 */
UCLASS()
class MULTSTRAT_API ADMBaseGalaxyObject : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	/** Constructor: Instantiate components */
	ADMBaseGalaxyObject(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/** Team that owns the ship and can issue it commands */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDMTeamComponent> TeamComponent;
	
	/** Team that owns the ship and can issue it commands */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UDMActiveCommandsComponent> CommandsComponent;
	
};
