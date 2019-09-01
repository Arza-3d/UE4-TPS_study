#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Enum/AmmoAndEnergyEnum.h"
#include "Enum/RangedWeaponEnum.h"
#include "RangedWeaponStruct.generated.h"

USTRUCT(BlueprintType)
struct FWeapon
{
	GENERATED_BODY();

	/** name/names of the socket where the projectile is spawned*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		TArray<FName> SocketName = { FName(TEXT("Muzzle_01")) };

	/**
	 * 0 = fire rate,
	 * 1 = max hold rate,
	 * 2 = reload time,
	 * 3 = equip time,
	 * 4 = unequip time
	 * all of that are in second
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> FireRateAndOther = { 0.3f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	ETriggerMechanism Trigger;

	/* is the weapon cost int (ammo), or float (energy), or nothing (unlimited)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EWeaponCost WeaponCost;

	/** used only if WeaponCost is "Ammo"*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EAmmoType AmmoType;

	/** used only if WeaponCost is "Energy"*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EEnergyType EnergyType;

	/** used only if WeaponCost is "Energy", this is in %*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EnergyUsePerShot = 10.0f;
};



UCLASS()
class TPS_STUDY_API URangedWeaponStruct : public UObject
{
	GENERATED_BODY()
	
};
