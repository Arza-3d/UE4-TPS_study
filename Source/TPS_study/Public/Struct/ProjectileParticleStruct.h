// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ProjectileParticleStruct.generated.h"

class UParticleSystem;

USTRUCT(BlueprintType)
struct FProjectileParticle
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> MuzzleVX = { nullptr };

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> TrailVX = { nullptr };

	/**
	 * 0 = HitWorld
	 * 1 = HitCharacter
	 * 2 = HiWater
	 * 3 = NoHit
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> HitVX = { nullptr };

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
UCLASS()
class TPS_STUDY_API UProjectileParticleStruct : public UObject
{
	GENERATED_BODY()
	
};
