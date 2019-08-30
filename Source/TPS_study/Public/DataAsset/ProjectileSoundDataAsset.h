// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Struct/ProjectileSoundStruct.h"
#include "ProjectileSoundDataAsset.generated.h"



/**
 * 
 */
UCLASS(BlueprintType)
class TPS_STUDY_API UProjectileSoundDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProjectileSound ProjectileSoundEffect;

};
