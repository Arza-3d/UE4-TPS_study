#include "AimingComponent.h"

//===========================================================================
// public function:
//===========================================================================

UAimingComponent::UAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

//===========================================================================
// protected function:
//===========================================================================

void UAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
