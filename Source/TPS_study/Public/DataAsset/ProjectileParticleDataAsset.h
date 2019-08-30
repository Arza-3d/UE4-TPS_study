// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Struct/ProjectileParticleStruct.h"
#include "ProjectileParticleDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TPS_STUDY_API UProjectileParticleDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProjectileParticle ProjectileParticle;
	
};
