// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AimingEnum.generated.h"

//=============
// Aiming Enum:
//=============

UENUM(BlueprintType)
enum class EAimingState : uint8
{
	NotAiming,
	TransitioningAiming,
	Aiming
};

/**
 * 
 */
UCLASS()
class TPS_STUDY_API UAimingEnum : public UObject
{
	GENERATED_BODY()
	
};
