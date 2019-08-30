// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmmoAndEnergyEnum.generated.h"

UENUM(BlueprintType)
enum class EWeaponCost : uint8
{
	Nothing,
	Ammo,
	Energy
};

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	StandardAmmo,
	RifleAmmo,
	ShotgunAmmo,
	Rocket,
	Arrow,
	Grenade,
	Mine
};

UENUM(BlueprintType)
enum class EEnergyType : uint8
{
	MP,
	Fuel,
	Battery,
	Overheat
};

/**
 * 
 */
UCLASS()
class TPS_STUDY_API UAmmoAndEnergyEnum : public UObject
{
	GENERATED_BODY()
	
};
