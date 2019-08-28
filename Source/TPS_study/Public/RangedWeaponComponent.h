#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSFunctionLibrary.h"
#include "RangedWeaponComponent.generated.h"

class UCurveFloat;
class UDataTable;
class UUserWidget;
class ATPShooterCharacter;
class ATPS_studyCharacter;

UENUM(BlueprintType)
enum class EAimingState : uint8
{
	NotAiming,
	TransitioningAiming,
	Aiming
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiming, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitioningAiming, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopAiming, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWeapon, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireSignature, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunOutOfAmmoSignature, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsOverheatingSignature, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnNoMoreAmmoDuringFire, URangedWeaponComponent*, MyComponent, const int32, MyFireRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMaxFireHoldRelease, URangedWeaponComponent*, MyComponent, const float, MyCurrentHoldTime, const float, MyMaxHoldTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRunOutOfEnergySignature, URangedWeaponComponent*, MyComponent, const float, CurrentEnergy, const float, EnergyNeededPerShot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnJustReachMaxFireHoldSignature, URangedWeaponComponent*, MyComponent, const float, MyCurrentHoldTime, const float, MyMaxHoldTime);

//=============================================================================
/**
 * RangedWeaponComponent handles projectile based attack logic
 * It contains:
 *  * Weapon table
 *  * Aiming table
 *  * Aiming functionality
 *  * Fire functionality
 *  * Switch weapon functionality
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API URangedWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	friend ATPS_studyCharacter;
	friend ATPShooterCharacter;

//===========================================================================
public:	
//===========================================================================

	URangedWeaponComponent();

	//================
	// Event (public):
	//================

	/** Called when actor reach aiming state */
	UPROPERTY(BlueprintAssignable, Category = "Aiming")
	FOnAiming OnAiming;

	/** Called when actor in transitioning in aiming state */
	UPROPERTY(BlueprintAssignable, Category = "Aiming")
	FOnTransitioningAiming OnTransitioningAiming;

	/** Called when actor reach aiming state */
	UPROPERTY(BlueprintAssignable, Category = "Aiming")
	FOnStopAiming OnStopAiming;

	/** Called when actor fire the weapon */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnFireSignature OnFire;

	/** Called when max fire hold is reach */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnJustReachMaxFireHoldSignature OnJustReachMaxHoldTrigger;

	/** Called when actor can't shoot more during multiple shot  */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnMaxFireHoldRelease OnMaxFireHoldRelease;

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

	/** Called when actor switch weapon */
	UPROPERTY(BlueprintAssignable, Category = "Switch Weapon Event")
	FOnSwitchWeapon OnSwitchWeapon;

	//=================
	// Getter (public):
	//=================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int32 GetWeaponIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int32 GetLastWeaponIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FName GetWeaponName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	ETriggerMechanism GetTriggerMechanism() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	FAmmoCount GetAllAmmo() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	float GetAimingAlpha() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetIsAiming() const;

	//=================
	// Setter (public):
	//=================

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool bInBool);

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void AddAmmo(const EAmmoType InAmmoType, const int32 AdditionalAmmo);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddEnergy(const EEnergyType InEnergyType, const float AdditionalEnergy);

	//==================================
	// Function for Controller (public):
	//==================================

	UFUNCTION(BlueprintCallable, Category ="Fire")
	void FirePress();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingPress();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingRelease();

	//===============================
	// Default variables (protected)
	//===============================

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<UUserWidget> Crosshair;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	UDataTable* AimingTable;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	bool bIsAbleToShootWithoutAiming;

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	//===============================
	// Default Variables (protected):
	//===============================

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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UDataTable* WeaponTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	float TotalAimingTime = 0.8f;

	/** 
	 * Curve time must be 0 to 1
	 * Curve value muse be 0 to 1
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Aiming", Meta = (DisplayName = "Aiming Curve 0 -> 1", DisplayPriority = "1"))
	UCurveFloat* AimingCurve;
	
//===========================================================================
private:
//===========================================================================

	//========
	// Aiming:
	//========

	void TimeAiming(float InAlpha);

	// 0 = default, 1 = aiming, 2, 3, x extra mode
	TArray<FAimingStat> AimStats;
	int32 AimStatStartIndex = 0;
	int32 AimStatTargetIndex = 1;
	TArray<FName> AimingNames;
	void OrientCharacter(const bool bMyCharIsAiming);

	//========================
	// Shotter stat (private):
	//========================

	ATPShooterCharacter* Shooter;
	ACharacter* MyCharacter;

	//=======================
	// Weapon stat (private):
	//=======================

	FAmmoCount AmmunitionCount;
	FExternalEnergyCount EnergyExternal;

	FWeapon CurrentWeapon;
	FProjectile CurrentProjectile;

	int32 WeaponIndex;
	int32 LastWeaponIndex;
	TArray<FName> WeaponNames;
	
	//==================
	// Aiming (private):
	//==================

	EAimingState AimingState = EAimingState::NotAiming;

	bool bMaxHoldIsReach;
	float MaxFireHoldTime;

	FTimerHandle AimingTimerHandle;
	float DeltaSecond;
	bool bIsAimingForward;

	void AimingTimerStart();
	void ClearAndStartAimingTimer();

	void ClearAndInvalidateAimingTimer(const float NewCurrentTime);
	//bool bIsTransitioningAiming;
	float AimingAlpha;
	float CurrentAimingTime;
	
	//================
	// Fire (private):
	//================

	USceneComponent* WeaponInWorld;
	void SetWeaponMesh();

	void SetWeaponMode(const int32 MyWeaponIndex);
	void SetWeaponIndex(const int32 InNumber);
	void SetWeaponIndex(const bool isUp);

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType InAmmoType);
	bool IsAmmoEnough(const EEnergyType InEnergyType);
	bool IsAmmoEnough(const int32 InAmmo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();

	bool bIsFireRatePassed = true;
	bool bIsTriggerPressed;
	bool bOnePressToggle;

	void FireStandardTrigger();
	void FireAutomaticTrigger();
	void FireHold();
	float HoldTime;
	float HoldTimeRateCount = 0.2f;
	void FireAutomaticTriggerOnePress();

	void FireReleaseAfterHold();
	void FlipOnePressTriggerSwitch();
	bool IsWeaponAbleToFire();

	void FireProjectile();
	void FireProjectile(const EAmmoType AmmoType);
	void FireProjectile(const EEnergyType EnergyType);
	void FireProjectile(int* Ammo);
	void FireProjectile(float* Energy);
	void SpawnProjectile(USceneComponent* WeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int32 i);

	FTimerHandle TimerOfHoldTrigger;
	void CountHoldTriggerTime();

	FTimerHandle FireRateTimer;
	void TimerFireRateStart();
	void TimerFireRateReset();

	FRotator GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform);
	void PlayFireMontage();

};
