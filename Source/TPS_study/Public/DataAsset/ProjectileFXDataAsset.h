#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ProjectileFXDataAsset.generated.h"

class UParticleSystem;

USTRUCT(BlueprintType)
struct FProjectileVX
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> MuzzleVX = {nullptr};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> TrailVX = {nullptr};

	/**
	 * 0 = HitWorld
	 * 1 = HitCharacter
	 * 2 = HiWater
	 * 3 = NoHit
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> HitVX = {nullptr};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UParticleSystem* HitCharacter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UParticleSystem* HitWater;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UParticleSystem* NoHit;
};

/**
 * 
 */
UCLASS(BlueprintType)
class TPS_STUDY_API UProjectileFXDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProjectileVX ProjectileVX;
};
