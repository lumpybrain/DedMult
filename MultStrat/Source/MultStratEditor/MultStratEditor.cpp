// Copyright Epic Games, Inc. All Rights Reserved.

// Copyright (c) 2025 William Pritz under MIT License

#include "MultStratEditor.h"

#include "DMEVisualizerComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "Components/DMNodeConnectionComponent.h"	// UDMNodeConnectionComponent
#include "Modules/ModuleManager.h"					// IMPLEMENT_PRIMARY_GAME_MODULE
#include "UnrealEdGlobals.h"

/**
* Implement the MultStrat Editor Module's interface
*/
class FMultStratEditorModule : public IMultStratEditorModule
{
	//~ Begin IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMultStratEditorModule, MultStratEditor, "MultStratEditor" );

/******************************************************************************
 * IModuleInterface Override: Start the module on editor load
******************************************************************************/
void FMultStratEditorModule::StartupModule()
{
	if (GUnrealEd)
	{
		TSharedPtr<FDMEVisualizerComponent> Visualizer = MakeShareable(new FDMEVisualizerComponent());
		GUnrealEd->RegisterComponentVisualizer(UDMNodeConnectionComponent::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
}

/******************************************************************************
 * IModuleInterface Override: Shut down the module when editor is closed
******************************************************************************/
void FMultStratEditorModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UDMNodeConnectionComponent::StaticClass()->GetFName());
	}
}
