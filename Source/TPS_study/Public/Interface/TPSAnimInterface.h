// Arza3d

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TPSAnimInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UTPSAnimInterface : public UInterface
{
	GENERATED_BODY()
};

class TPS_STUDY_API ITPSAnimInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Navigation", meta = (Keywords = "Animation Interface"))
	void IsInAirIsSetTo(const bool inBool);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Navigation", meta = (Keywords = "Animation Interface"))
	void NormalizedForwardIsSetTo(const float normForward);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Navigation", meta = (Keywords = "Animation Interface"))
	void NormalizedRightIsSetTo(const float normRight);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon", meta = (Keywords = "Animation Interface"))
	void WeaponIndexIsSetTo(const int weaponIndex);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Weapon", meta = (Keywords = "Animation Interface"))
	void IsAimingIsSetTo(const bool inBool);
};
