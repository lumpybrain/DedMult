// Copyright (c) 2025 William Pritz under MIT License


#include "GalaxyObjects/DMBaseGalaxyObject.h"

#include "Components/DMTeamComponent.h"			// UDMTeamComponent
#include "Components/DMCommandFlagsComponent.h"	// UDMActiveCommandsComponent


/******************************************************************************
 * Constructor: Instantiate components
******************************************************************************/
ADMBaseGalaxyObject::ADMBaseGalaxyObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamComponent = CreateDefaultSubobject<UDMTeamComponent>(TEXT("Team Component"));
	CommandsComponent = CreateDefaultSubobject<UDMActiveCommandsComponent>(TEXT("Active Commands Component"));
}
