#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectileSXDataAsset.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FProjectileSX
{
	GENERATED_BODY();

	/** Muzzle sound effect*/
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USoundBase* MuzzleSX;

	/**
	 * 0 = HitWorld
	 * 1 = HitCharacter
	 * 2 = HiWater
	 * 3 = NoHit
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USoundBase* HitSX;

	/**
	 * 0 = HitWorld
	 * 1 = HitCharacter
	 * 2 = HiWater
	 * 3 = NoHit
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USoundBase* TrailVX;
};

/**
 * Data asset that contain the sound of the object
 */
UCLASS()
class TPS_STUDY_API UProjectileSXDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProjectileSX ProjectileSoundEffect;
};
