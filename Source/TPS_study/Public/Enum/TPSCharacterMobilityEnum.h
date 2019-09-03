#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TPSCharacterMobilityEnum.generated.h"

/*UENUM(BlueprintType)
enum class ETPSBlendSpace : uint8
{
	Idle,
	Jog,
	Sprint
};*/

UENUM(BlueprintType)
enum class ETPSJogBlendSpace : uint8
{
	Idle,
	Forward,
	Backward,
	Right,
	Left,
	ForwardRight,
	ForwardLeft,
	BackwardRight,
	BackwardLeft
};

UCLASS()
class TPS_STUDY_API UTPSCharacterMobilityEnum : public UObject
{
	GENERATED_BODY()
	
};
