#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "TPSFunctionLibrary.h"
#include "TPShooterCharacter.generated.h"

class UDataTable;
class UUserWidget;
class USpringArmComponent;
class UCameraComponent;
class URangedWeaponComponent;
class ATPS_Projectile;

UCLASS(config = Game)
class ATPShooterCharacter : public ACharacter {

	GENERATED_BODY()

	friend URangedWeaponComponent;

//===========================================================================
public:
//===========================================================================

	ATPShooterCharacter();

	//==============================
	// Blueprint Component getter
	//==============================

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE URangedWeaponComponent* GetRangedWeapon() const { return RangedWeapon; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetIsAiming() const;

	float GetHP() const;

	void SetHP(float val);

	float GetMP() const;

	void SetMP(float val);

	UFUNCTION(BlueprintCallable)
	void AddAmmo(const int32 addAmmo, const EAmmoType ammoType);

//===========================================================================
protected:
//===========================================================================

	//===============================
	// Default variables (protected)
	//===============================

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<UUserWidget> Crosshair;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	UDataTable* AimingTable;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	bool bWeaponIsAlwaysAiming;

	//====================
	// Getter (protected)
	//====================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	ETriggerMechanism GetTriggerMechanism() const;

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	//FName GetWeaponName() const;

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	//int32 GetWeaponIndex() const;

	//UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	//int32 GetLastWeaponIndex() const;

	//======================
	// Function (protected)
	//======================

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FirePress();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingPress();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingRelease();

	//===================
	// Event (protected)
	//===================

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Aiming")
	void OnAiming();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Aiming")
	void OnStopAiming();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponFires();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnMaxFireHoldRelease();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnMaxFireHold();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoAmmo();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoEnergy();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponOverheats();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoMoreAmmoDuringFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (KeyWords = "change interface"))
	void OnSwitchWeapon();

	//================================
	// overriden function (protected)
	//================================

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToSwitchWeapon();
	virtual bool IsAbleToSwitchWeapon_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Aiming")
	bool IsAbleToAim();
	virtual bool IsAbleToAim_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToRepeatAutoFire();
	virtual bool IsAbleToRepeatAutoFire_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Fire")
	bool IsAbleToFire();
	virtual bool IsAbleToFire_Implementation();

	/////////////
	// and other
	/////////////

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool bInBool);

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

//===========================================================================
private:
//===========================================================================

	//======================
	// Blueprint Component:
	//======================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	URangedWeaponComponent* RangedWeapon;

	UPROPERTY()
	UTimelineComponent* AimingTimeline;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	UCurveFloat* FloatCurve;

	UFUNCTION()
	void TimeAiming(float InAlpha);

	UFUNCTION()
	void TimeFinishAiming();

	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimeAimingDirection;

	//========
	// Aiming:
	//========

	// 0 = default, 1 = aiming, 2, 3, x extra mode
	TArray<FAimingStat> AimStats;
	int32 AimStatStartIndex = 0;
	int32 AimStatTargetIndex = 1;
	TArray<FName> AimingNames;

	bool bIsAiming;
	bool bIsTransitioningAiming;
	bool bMaxHoldIsReach;
	float MaxFireHoldTime;

	void Aiming(const bool bInIsAiming);

	void OrientCharacter(const bool bMyCharIsAiming);

	FCharacterStat CharacterStat;
	FAmmoCount Ammunition;
	FExternalEnergyCount EnergyExternal;

	FShooter ShooterState;
	FWeapon CurrentWeapon;
	FProjectile CurrentProjectile;

	//======
	// Fire:
	//======

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

	//===============
	// Switch Weapon:
	//===============

	// variables:
	//TArray<FName> WeaponNames;
	//USceneComponent* WeaponInWorld;

	// function:
	//void SetWeaponMesh();

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType ammo);
	bool IsAmmoEnough(const EEnergyType EnergyType);
	bool IsAmmoEnough(const int32 InAmmo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();

	//void SetWeaponIndex(const int32 InNumber);
	void SetWeaponIndex1();
	void SetWeaponIndex2();
	void SetWeaponIndex3();
	void SetWeaponIndex4();

	//void SetWeaponIndex(const bool isUp);
	void SetWeaponIndexUp();
	void SetWeaponIndexDown();

	//void SetWeaponMode(const int32 MyWeaponIndex);

	///////////////
	// Navigation
	///////////////

	// variables:
	const float BaseTurnRate = 45.0f;
	const float BaseLookUpRate = 45.0f;

	bool bForwardInputPressed;
	bool bRightInputPressed;

	float NormalizedForward;
	float NormalizedRight;

	float AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRightAtRate(float Rate);
	void LookUpAtRate(float Rate);
};