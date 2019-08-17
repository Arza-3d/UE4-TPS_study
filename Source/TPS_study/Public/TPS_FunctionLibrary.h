#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystem.h"
#include "TPS_FunctionLibrary.generated.h"

class UAnimMontage;
class USoundBase;
class UParticleSystem;

/**
 * Will put some extra struct and enum in this class
 */
UCLASS()
class TPS_STUDY_API UTPS_FunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()
		UParticleSystem* GetRandomParticle(TArray<UParticleSystem*> particleSystems);
};

// 0 character state
UENUM(BlueprintType)
enum class ECharacterMobility : uint8 {
	Idle,
	Jog,
	Sprint,
	Crouch,
	Jump,
	Ragdoll
};
UENUM(BlueprintType)
enum class ECharacterHealthState : uint8 {
	Idle,
	Damaged,
	Stunned,
	Died
};
UENUM(BlueprintType)
enum class ECharacterShooterState : uint8 {
	Idle,
	Aiming,
	Shooting
};

// 1.a weapon table
UENUM(BlueprintType)
enum class ETriggerMechanism : uint8 {
	PressTrigger,
	ReleaseTrigger,
	AutomaticTrigger,
	OnePressAutoTrigger
};
UENUM(BlueprintType)
enum class EWeaponCost : uint8 {
	Nothing,
	Ammo,
	Energy,
	Overheat
};
UENUM(BlueprintType)
enum class EAmmoType : uint8 {
	StandardAmmo,
	RifleAmmo,
	ShotgunAmmo,
	Rocket,
	Arrow,
	Grenade,
	Mine
};
USTRUCT(BlueprintType)
struct FCeiledFloat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Value = 100.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Max = 100.0f;
};
USTRUCT(BlueprintType)
struct FCharacterStat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	FCeiledFloat Health;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	FCeiledFloat Mana;
};
USTRUCT(BlueprintType)
struct FAmmoCount {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
	int StandardAmmo = 50;
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
struct FShooter {
	GENERATED_BODY();
	/**
	* 0 = FireMontage,
	* 1 = EquipMontage,
	* 2 = UnequipMontage
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TArray<UAnimMontage*> CharacterWeaponMontage;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	USoundBase* FireCry;
};
USTRUCT(BlueprintType)
struct FWeapon {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TArray<FName> SocketName;
	/**
	* 0 = fire rate,
	* 1 = reload time,
	* 2 = equip time,
	* 3 = unequip time
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	TArray<float> FireRateAndOther;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
	ETriggerMechanism Trigger;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
	EWeaponCost WeaponCost;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
	EAmmoType AmmoType;
	/**
	* 0 = current ammo,
	* 1 = max ammo before reload
	* 2 = max ammo that can be carried
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
	TArray<int> Ammo;
	FWeapon() {
		SocketName.Add(FName(TEXT("Muzzle_01")));
		FireRateAndOther.Add(0.5f);
	}
};
USTRUCT(BlueprintType)
struct F_FX {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	USoundBase* SoundEffect;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	TArray<UParticleSystem*> VisualEffect;
	F_FX() { VisualEffect.Add(nullptr); }
};
USTRUCT(BlueprintType)
struct FProjectileMuzzle {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	F_FX MuzzleFX;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
	TEnumAsByte<ECollisionChannel> CollisionComp;
	/**
	* 0 = InitialSpeed,
	* 1 = ProjectileMultiplier,
	* 2 = GravityScale
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Physics")
	TArray<float> InitialSpeedAndOther = { 1200.0f };
};
USTRUCT(BlueprintType)
struct FProjectileTrail {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	F_FX TrailFX;
};
USTRUCT(BlueprintType)
struct FProjectileHit {
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
struct FProjectile {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	FProjectileMuzzle Muzzle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	FProjectileTrail Trail;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	FProjectileHit Hit;
};
USTRUCT(BlueprintType)
struct FWeaponMode {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FShooter Shooter;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWeapon Weapon;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FProjectile Projectile;
};
USTRUCT(BlueprintType)
struct FWeaponModeCompact : public FTableRowBase {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWeaponMode WeaponMode;
};
// 1.z weapon table

// 2.a Aiming
USTRUCT(BlueprintType)
struct FCharMovAimingStat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxAcceleration = 2048.0f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxWalkSpeed = 600.0f;
};
USTRUCT(BlueprintType)
struct FCamBoomAimingStat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SocketOffset;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TargetArmLength = 300.0f;
};
USTRUCT(BlueprintType)
struct FllowCamAimingStat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView = 90.0f;
};
USTRUCT(BlueprintType)
struct FAimingStat {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCharMovAimingStat CharMov;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCamBoomAimingStat CamBoom;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FllowCamAimingStat FollCam;
};
USTRUCT(BlueprintType)
struct FAimingStatCompact : public FTableRowBase {
	GENERATED_BODY();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAimingStat AimStat;
};
// 2.z Aiming