// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "Enum/AmmoAndEnergyEnum.h"
#include "Enum/RangedWeaponEnum.h"
#include "WeaponTableStruct.generated.h"


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
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		TArray<float> FireRateAndOther = { 0.3f };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		ETriggerMechanism Trigger;

	/* is the weapon cost int (ammo), or float (energy), or nothing (unlimited)*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		EWeaponCost WeaponCost;

	/** used only if WeaponCost is "Ammo"*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		EAmmoType AmmoType;

	/** used only if WeaponCost is "Energy"*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		EEnergyType EnergyType;

	/** used only if WeaponCost is "Energy", this is in %*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		float EnergyUsePerShot = 10.0f;
};

USTRUCT(BlueprintType)
struct F_FX
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		USoundBase* SoundEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		TArray<UParticleSystem*> VisualEffect = { nullptr };
};

USTRUCT(BlueprintType)
struct FProjectileMuzzle
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		F_FX MuzzleFX;

	/** Should be setup as PlayerProjectile collision channel */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		TEnumAsByte<ECollisionChannel> CollisionComp;

	/**
	* 0 = InitialSpeed,
	* 1 = ProjectileMultiplier,
	* 2 = GravityScale
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		TArray<float> InitialSpeedAndOther = { 9000.0f };
};
USTRUCT(BlueprintType)
struct FProjectileTrail
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		F_FX TrailFX;
};

USTRUCT(BlueprintType)
struct FProjectileHit
{
	GENERATED_BODY();

	/**
	* 0 = Hit FX,
	* 1 = AoE FX
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		TArray<F_FX> HitFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		float DamagePoint = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		float CriticalChance = 0.05f;
};

USTRUCT(BlueprintType)
struct FProjectile
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		FProjectileMuzzle Muzzle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		FProjectileTrail Trail;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		FProjectileHit Hit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		class UProjectileFXDataAsset* ProjectileVX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		class UProjectileSXDataAsset* ProjectileSoundEffect;
};

USTRUCT(BlueprintType)
struct FWeaponMode
{
	GENERATED_BODY();

	//UPROPERTY(BlueprintReadWrite, EditAnywhere)
	//FShooter Shooter;

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
