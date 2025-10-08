// Copyright Epic Games, Inc. All Rights Reserved.

#include "MultStratEditor.h"

#include "DMEVisualizerComponent.h"
#include "Editor/UnrealEdEngine.h"
#include "GalaxyObjects/DMGalaxyNode.h"
#include "Modules/ModuleManager.h"
#include "UnrealEdGlobals.h"

class FMultStratEditorModule : public IMultStratEditorModule
{
	// Begin IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_PRIMARY_GAME_MODULE(FMultStratEditorModule, MultStratEditor, "MultStratEditor" );

void FMultStratEditorModule::StartupModule()
{
	if (GUnrealEd)
	{
		TSharedPtr<FDMEVisualizerComponent> Visualizer = MakeShareable(new FDMEVisualizerComponent());
		GUnrealEd->RegisterComponentVisualizer(UDMNodeConnectionManager::StaticClass()->GetFName(), Visualizer);
		Visualizer->OnRegister();
	}
}

void FMultStratEditorModule::ShutdownModule()
{
	if (GUnrealEd)
	{
		GUnrealEd->UnregisterComponentVisualizer(UDMNodeConnectionManager::StaticClass()->GetFName());
	}
}
