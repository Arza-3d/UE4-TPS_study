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
	AutomaticTrigger
};

UENUM(BlueprintType)
enum class EWeaponCost : uint8
{
	None,
	Ammo,
	Energy,
	Overheat
};

USTRUCT(BlueprintType)
struct FShooter
{
	GENERATED_BODY();
	
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
	EWeaponCost WeaponLimit = EWeaponCost::None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Logic")
	int LimitCost = 6;

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
struct FWeaponMode : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FShooter Shooter;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FWeapon Weapon;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FProjectile Projectile;
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

protected:

	UFUNCTION(BlueprintCallable)
	void MainFire(int weaponIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming", meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	float GetNewPlayRate(UAnimMontage* animMontage, float fireRate);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void SetIsTriggerPressed(bool bTriggerPressed);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation", meta = (ToolTip = "only used for aim anim blend walk"))
	float GetNormalizedForward();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation", meta = (ToolTip = "only used for aim anim blend walk"))
	float GetNormalizedRight();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponTable;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////E-a
	// EVENT
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (ToolTip = "regular weapon fire"))
	void Fire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (ToolTip = "automatic weapon fire"))
	void AutoFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (ToolTip = "repeat fire for automatic weapon"))
	void RepeatFire();
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////E-z

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	bool GetIsFireRatePassed();

	UFUNCTION(BlueprintCallable, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	void SetIsFireRatePassed(bool bFireRatePassed);

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void Aiming(bool bIsCharAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	bool IsWeaponNameInThisIndexExist(int weaponIndex);
	
	////////////////////////////////////////////////////////////////////////////////////////////BPN-a
	// BPNativeEvent
	////////////////////////////////////////////////////////////////////////////////////////////
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsSwitchWeaponRequirementFulfilled();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToRepeatAutoFire();
	////////////////////////////////////////////////////////////////////////////////////////////BPN-a

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNames;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponIndex(int weaponIndex);

	/** Test this comment **/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetWeaponIndex();

	int WeaponIndex;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetLastWeaponIndex(int lastWeaponIndex);

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