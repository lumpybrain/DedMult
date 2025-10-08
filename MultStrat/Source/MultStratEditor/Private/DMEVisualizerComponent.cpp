

#include "DMEVisualizerComponent.h"

#include "GalaxyObjects\DMGalaxyNode.h"


// ----------------------------------------------------------------------------
void FDMEVisualizerComponent::DrawVisualization(const UActorComponent* Component, const FSceneView* View,
	FPrimitiveDrawInterface* PDI) /*override*/
{
	if (!IsValid(Component))
	{
		return;
	}
	
	const AActor* Owner = Component->GetOwner();
	if (!IsValid(Owner))
	{
		return;
	}

	const UDMNodeConnectionManager* ManagerComponent = Cast<UDMNodeConnectionManager>(Component);
	if (!IsValid(ManagerComponent))
	{
		return;
	}

	// Draw a connection for each connected node
	FVector OwnerLocation = Owner->GetActorLocation();
	for (auto iter : ManagerComponent->ConnectedNodes)
	{
		if (!IsValid(iter))
		{
			continue;
		}

		// TODO: Customizable colors in .ini?
		PDI->DrawLine(OwnerLocation, iter->GetActorLocation(), FLinearColor::Red, 0, 5);
	}
}