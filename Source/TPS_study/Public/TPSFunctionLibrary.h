#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TPSFunctionLibrary.generated.h"

class UAnimMontage;
class UCurveFloat;
class USoundBase;
class UParticleSystem;
class UProjectileFXDataAsset;
class UProjectileSXDataAsset;

/*USTRUCT(BlueprintType)
struct FMinMaxFloat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Min;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	float Max = 1.0;
};

USTRUCT(BlueprintType)
struct FMinMaxCurve
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FMinMaxFloat TimeRange;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FMinMaxFloat FloatRange;
};*/

/**
 * Will put some extra struct and enum in this class
 */
UCLASS()
class TPS_STUDY_API UTPSFunctionLibrary : public UBlueprintFunctionLibrary {
	GENERATED_BODY()

public:

	static float GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage);

	static UParticleSystem* GetRandomParticle(TArray<UParticleSystem*> particleSystems);

	static float  StandardLinearInterpolation(const float X, const float X1, const float X2, const float Y1, const float Y2);

	//static FMinMaxCurve GetCurveRange(const UCurveFloat* InCurve);
};

// 0 character state
UENUM(BlueprintType)
enum class ECharacterMobility : uint8
{
	Idle,
	Jog,
	Sprint,
	Crouch,
	Jump,
	Ragdoll
};

UENUM(BlueprintType)
enum class ECharacterHealthState : uint8
{
	Idle,
	Damaged,
	Stunned,
	Died
};

UENUM(BlueprintType)
enum class ECharacterShooterState : uint8
{
	Idle,
	Aiming,
	Shooting
};

// 1.a weapon table
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

USTRUCT(BlueprintType)
struct FAmmoCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int StandardAmmo = 15;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int RifleAmmo = 30;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int ShotgunAmmo = 10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int Rocket = 3;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int Arrow = 18;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int Grenade = 2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
	int Mine = 3;
};

USTRUCT(BlueprintType)
struct FCharacterStatBPCPP
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	float HP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	float MP = 100.0f;
};

/**Only external energy mana is not included*/
USTRUCT(BlueprintType)
struct FExternalEnergyCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy", Meta = (ClampMin = "0"))
	float MP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Fuel = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Battery = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Overheat = 0.0f;
};

USTRUCT(BlueprintType)
struct FShooter
{
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
	UProjectileFXDataAsset* ProjectileVX;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Effect")
	UProjectileSXDataAsset* ProjectileSoundEffect;
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

//==========
// Aiming :
/////////////
USTRUCT(BlueprintType)
struct FCharMovAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxAcceleration = 2048.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float MaxWalkSpeed = 600.0f;
};

USTRUCT(BlueprintType)
struct FCamBoomAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector SocketOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TargetArmLength = 300.0f;
};

USTRUCT(BlueprintType)
struct FllowCamAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float FieldOfView = 90.0f;
};

USTRUCT(BlueprintType)
struct FAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCharMovAimingStat CharMov;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCamBoomAimingStat CamBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FllowCamAimingStat FollCam;
};

USTRUCT(BlueprintType)
struct FAimingStatCompact : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FAimingStat AimStat;
};