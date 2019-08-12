// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "TPS_Weapon.generated.h"

UENUM(BlueprintType)
enum class ETriggerMechanism : uint8
{
	PressTrigger,
	ReleaseTrigger,
	AutomaticTrigger,
	OnePressAutoTrigger
};

UENUM(BlueprintType)
enum class EWeaponCost : uint8
{
	Nothing,
	Ammo,
	Energy,
	Overheat
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

USTRUCT(BlueprintType)
struct FCeiledFloat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Value = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float Max = 100.0f;
};

USTRUCT(BlueprintType)
struct FCeiledInt
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Value = 36;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int Max = 36;
};

USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
		FCeiledFloat Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
		FCeiledFloat Mana;
};

USTRUCT(BlueprintType)
struct FAmmoCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int StandardAmmo;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int RifleAmmo;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int ShotgunAmmo;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int Rocket;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int Arrow;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int Grenade;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
		int Mine;
};

USTRUCT(BlueprintType)
struct FShooter
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* FireMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* EquipMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* UnequipMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		USoundBase* FireCry;
};

USTRUCT(BlueprintType)
struct FWeapon
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		TArray<FName> SocketName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		float FireRate = 0.5f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		ETriggerMechanism Trigger;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		EWeaponCost WeaponCost;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		EAmmoType AmmoType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		int AmmoCapacity = 6;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		float ReloadTime = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
		float EquipTime = 1.5f;

	FWeapon()
	{
		SocketName.Add(FName(TEXT("Muzzle01")));
	}
};

USTRUCT(BlueprintType)
struct F_FX
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		USoundBase* SoundEffect;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		TArray<UParticleSystem*> VisualEffect;

	F_FX()
	{
		VisualEffect.Add(nullptr);
	}
};

USTRUCT(BlueprintType)
struct FProjectileMuzzle
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		F_FX MuzzleFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		TEnumAsByte<ECollisionChannel> CollisionComp;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		float ProjectileMultiplier = 1.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		float InitialSpeed = 1200.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
		bool bIsAffectedByGravity;
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

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		F_FX HitFX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
		F_FX AoEFX;

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
};

USTRUCT(BlueprintType)
struct FWeaponMode
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FShooter Shooter;

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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UTPS_Weapon : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTPS_Weapon();		
};
