#include "AimingComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TPShooterCharacter.h"

//===========================================================================
// public function:
//===========================================================================

UAimingComponent::UAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ATPShooterCharacter* character1 = Cast<ATPShooterCharacter>(GetOwner());
	character1->GetCameraBoom()->SocketOffset;

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
