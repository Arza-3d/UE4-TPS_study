// Arza.3d

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
//#include "Components/TimelineComponent.h"
#include "TPS_studyCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterLocomotionState : uint8
{
	Idle,
	Jog,
	Run,
	Sprint,
	Ragdoll
};

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
struct FAmmoCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int StandardAmmo;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int RifleAmmo;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int ShotgunAmmo;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int Rocket;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int Arrow;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
	int Grenade;

	UPROPERTY(BlueprintReadWrite, Category = "Ammo")
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

UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UTimeLineComponent* CPPAimingTimeline;

public:

	ATPS_studyCharacter();

	bool bIsFireRatePassed;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

protected:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ammo")
	FAmmoCount Ammunition;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* WeaponTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* WeaponModeTable;

	FShooter ShooterState;

	FWeapon CurrentWeapon;

	FProjectile CurrentProjectile;

	UFUNCTION(BlueprintCallable)
	void GetCurrentWeaponMode(int weaponIndex);

	UFUNCTION(BlueprintCallable)
	void MainFire(bool isTriggerPressed);

	void StandardFire(bool pressed);
	void AutomaticFire(bool pressed);
	void HoldReleaseFire(bool pressed);
	void OnePressAutoFire(bool pressed);



	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming", meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	float GetNewPlayRate(UAnimMontage* animMontage, float fireRate);

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward();

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight();

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////E-a
	// EVENT
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/**for press trigger*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void Fire();

	/**for automatic trigger*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void AutoFire();

	/**repeat fire for automatic trigger*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void RepeatFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (KeyWords = "Switch change weapon"))
	void OnSwitchWeaponSuccess();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////E-z

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	bool GetIsFireRatePassed();

	UFUNCTION(BlueprintCallable, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	void SetIsFireRatePassed(bool bFireRatePassed);

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void Aiming(bool bIsCharAiming);

	UFUNCTION(BlueprintCallable, Category = "Weapon", meta = (KeyWords = "Switch Change Weapon"))
	void SetWeaponIndexWithNumpad(int numberInput);

	UFUNCTION(BlueprintCallable, Category = "Weapon", meta = (KeyWords = "Switch Change Weapon"))
	void SetWeaponIndexWithMouseWheel(bool isUp);
	
	////////////////////////////////////////////////////////////////////////////////////////////BPN-a
	// BPNativeEvent
	////////////////////////////////////////////////////////////////////////////////////////////

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsSwitchWeaponRequirementFulfilled();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToRepeatAutoFire();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Fire")
	bool CanCharacterFire();

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool CanWeaponFire();

	//UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Fire")
	bool IsAmmoEnough(EAmmoType ammo);

	////////////////////////////////////////////////////////////////////////////////////////////BPN-a

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNames;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetWeaponIndex();

	int WeaponIndex;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetLastWeaponIndex();

	int LastWeaponIndex;

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void SetProjectileMultiplier(float projectileMultiplier);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	float GetProjectileMultipler();

	float ProjectileMultiplier;


	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	bool bIsTriggerPressed;

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool isTransitioningAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming();

	bool bIsTransitioningAiming;
	

	//UPROPERTY(VisibleAnywhere, Category = Aiming)
	//class UTimelineComponent* AimingTransitionTimeline;

private:
	
	float NormalizedForward;

	float NormalizedRight;

	bool bIsAiming;

	void OrientCharacter(bool bMyCharIsAiming);
	
	bool bForwardInputPressed;

	bool bRightInputPressed;

	float AssignNormalizedVelo(float MyValue,  bool bOtherButtonPressed);

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};