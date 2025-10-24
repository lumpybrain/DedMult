// Copyright (c) 2025 William Pritz under MIT License

#pragma once

#include "ComponentVisualizer.h"

/**
 * Editor only: draws red lines between nodes to show their connections when they are selected
 */
class FDMEVisualizerComponent : public FComponentVisualizer
{
public:
	//~ Begin FComponentVisualizer Interface

	virtual void DrawVisualization(const UActorComponent* Component, const FSceneView* View,
		FPrimitiveDrawInterface* PDI) override;

	// Override this to draw on the editor's viewport
	/* virtual void DrawVisualizationHUD(const UActorComponent* Component, const FViewport* Viewport,
		const FSceneView* View, FCanvas* Canvas) override; */

	//~ End FComponentVisualizer Interface
};