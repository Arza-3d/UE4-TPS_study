#include "HPandMPComponent.h"

//===========================================================================
// public:
//===========================================================================

UHPandMPComponent::UHPandMPComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

//===========================================================================
// protected:
//===========================================================================

// Called when the game starts
void UHPandMPComponent::BeginPlay()
{
	Super::BeginPlay();

	TheChar = Cast<APawn>(GetOwner());
	
}



