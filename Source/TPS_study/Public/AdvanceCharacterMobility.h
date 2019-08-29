#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvanceCharacterMobility.generated.h"


UENUM(BlueprintType)
enum class ECharacterMobility : uint8
{
	Idle,
	Jog,
	Sprint,
	Crouch,
	Jump,
	Ragdoll
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UAdvanceCharacterMobility : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAdvanceCharacterMobility();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
