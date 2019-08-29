#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"

#include "AmmoAndEnergyComponent.generated.h"

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

// 1.a weapon table
UENUM(BlueprintType)
enum class ETriggerMechanism : uint8
{
	PressTrigger,
	ReleaseTrigger,
	AutomaticTrigger,
	OnePressAutoTrigger
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunOutOfAmmoSignature, UAmmoAndEnergyComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsOverheatingSignature, UAmmoAndEnergyComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoMoreAmmoDuringFire, const int32, MyFireRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRunOutOfEnergySignature, UAmmoAndEnergyComponent*, MyComponent, const float, CurrentEnergy, const float, EnergyNeededPerShot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UAmmoAndEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class URangedWeaponComponent;

public:	
	
	UAmmoAndEnergyComponent();

	/** Called when actor can't shoot more during multiple shot  */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnNoMoreAmmoDuringFire OnNoMoreAmmoDuringMultipleShot;

	/** Called when actor can't shoot due to no more ammo */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnRunOutOfAmmoSignature OnAmmoOut;

	/** Called when actor can't shoot due to not enough energy */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnRunOutOfEnergySignature OnEnergyOut;

	/** Called when actor can't shoot due to weapon overheating */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnIsOverheatingSignature OnOverhating;

	//=================
	// Setter (public):
	//=================

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void AddAmmo(const EAmmoType InAmmoType, const int32 AdditionalAmmo);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddEnergy(const EEnergyType InEnergyType, const float AdditionalEnergy);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	FAmmoCount GetAllAmmo() const;

	/**
	 * if member is 0 will use default value from Struct
	 *    and will have no max ammunition value
	 * -------------------------------------------------
	 * if member is not 0, then (optional):
	 *  * 0 = initial ammunition
	 *  * 1 = max ammunition
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
	TArray<FAmmoCount> AmmunitionLimit;

protected:
	
	virtual void BeginPlay() override;

public:	
	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	template <class ThisType>
	ThisType* GetThisType() const
	{
		TArray<UActorComponent*> myComponents = GetOwner()->GetComponents().Array();
		ThisType* returnedVal = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			returnedVal = Cast<ThisType>(myComponents[i]);
			if (returnedVal) break;
		}
		return returnedVal;
	}

	URangedWeaponComponent* RangedWeaponComponent;

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType InAmmoType);
	bool IsAmmoEnough(const EEnergyType InEnergyType);
	bool IsAmmoEnough(const int32 InAmmo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();

	//=======================
	// Weapon stat (private):
	//=======================

	FAmmoCount AmmunitionCount;
	FExternalEnergyCount EnergyExternal;
		
};
