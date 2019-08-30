#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RangedWeaponEnum.generated.h"

UENUM(BlueprintType)
enum class ETriggerMechanism : uint8
{
	PressTrigger,
	ReleaseTrigger,
	AutomaticTrigger,
	OnePressAutoTrigger
};

/**
 * 
 */
UCLASS()
class TPS_STUDY_API URangedWeaponEnum : public UObject
{
	GENERATED_BODY()
	
};
