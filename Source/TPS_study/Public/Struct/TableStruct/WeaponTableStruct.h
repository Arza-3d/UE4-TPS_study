#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"

#include "Enum/AmmoAndEnergyEnum.h"
#include "Enum/RangedWeaponEnum.h"
#include "Struct/RangedWeaponStruct.h"
#include "Struct/ProjectileStruct.h"

#include "WeaponTableStruct.generated.h"

class UProjectileParticleDataAsset;
class UProjectileSoundDataAsset;

USTRUCT(BlueprintType)
struct FWeaponMode
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWeapon Weapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FProjectile Projectile;
};

USTRUCT(BlueprintType)
struct FWeaponModeCompact : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWeaponMode WeaponMode;
};

UCLASS()
class TPS_STUDY_API UWeaponTableStruct : public UObject
{
	GENERATED_BODY()
	
};
