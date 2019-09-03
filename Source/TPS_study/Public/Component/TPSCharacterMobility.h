#pragma once

#include "CoreMinimal.h"
#include "Component/ComponentBase.h"
#include "TPSCharacterMobility.generated.h"


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
class TPS_STUDY_API UTPSCharacterMobility : public UComponentBase
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTPSCharacterMobility();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

};
