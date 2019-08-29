#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPSFunctionLibrary.generated.h"

class UAnimMontage;
class UCurveFloat;
class USoundBase;
class UParticleSystem;
class UProjectileFXDataAsset;
class UProjectileSXDataAsset;

/**
 * Will put some extra struct and enum in this class
 */
UCLASS()
class TPS_STUDY_API UTPSFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:

	static float GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage);

	static UParticleSystem* GetRandomParticle(TArray<UParticleSystem*> particleSystems);

	static float  StandardLinearInterpolation(const float X, const float X1, const float X2, const float Y1, const float Y2);
};

// 0 character state

UENUM(BlueprintType)
enum class ECharacterHealthState : uint8
{
	Idle,
	Damaged,
	Stunned,
	Died
};

UENUM(BlueprintType)
enum class ECharacterShooterState : uint8
{
	Idle,
	Aiming,
	Shooting
};

