

#pragma once

#include "ComponentVisualizer.h"

class FDMEVisualizerComponent : public FComponentVisualizer
{
public:
	// Override this to draw in the scene
	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View,
		FPrimitiveDrawInterface* PDI) override;

	// Override this to draw on the editor's viewport
	/* virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport,
		const FSceneView* View, FCanvas* Canvas) override; */
};