#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "TPS_FunctionLibrary.h"
#include "TPS_studyCharacter.generated.h"

UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter {
	GENERATED_BODY()

	// 0.a CONSTRUCTION
public:
	ATPS_studyCharacter();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangedWeapon", meta = (AllowPrivateAccess = "true"))
	class UTPS_Weapon* RangedWeapon;
protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// 0.z CONSTRUCTION

	// 1.a NAVIGATION
public:
	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward();
	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight();
	// 1.z NAVIGATION

	// 2.a AIMING
protected:
	int AimStatStartIndex = 0;
	int AimStatTargetIndex = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<class UUserWidget> Crosshair;
	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void Aiming();
	void AimingStop();
	void Aiming_Setup(const bool isAiming);
	/**
	* 0 = default
	* 1 = aiming
	* 2, 3, x extra mode
	*/
	TArray<FAimingStat> AimStats;
	UPROPERTY()
	UTimelineComponent* AimingTimeline;
	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	UCurveFloat* FloatCurve;
	UFUNCTION()
	void TimeAiming(float val);
	UFUNCTION()
	void TimeFinishAiming();
	UPROPERTY()
	TEnumAsByte<ETimelineDirection::Type> TimeAimingDirection;
	// 2.z AIMING

	// 3.a FIRE
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming", meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed();
	UFUNCTION(BlueprintCallable)
	void Fire_Base(bool isTriggerPressed);
	UFUNCTION(BlueprintCallable)
	void SetWeaponMode(int weaponIndex);

	bool IsNoMoreAmmo();
	FRotator GetNewMuzzleRotation(FTransform socketTransform);
	void Fire__Standard(bool pressed);
	void Fire__Automatic(bool pressed);
	void Fire__HoldRelease(bool pressed);
	void Fire__AutomaticOnePress(bool pressed);
	void SpawnProjectile(USkeletalMeshComponent* weaponMesh);
	void ConsumeWeaponCost();
	void PlayFireMontage();
	
	// 3.z FIRE
public:
	FShooter ShooterState;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	FAmmoCount Ammunition;
	FWeapon CurrentWeapon;
	FProjectile CurrentProjectile;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* WeaponTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* WeaponModeTable;
	// 4.a SWITCH WEAPON

	// 4.z SWITCH WEAPON

	// 5.a PICKUP
public:
	UFUNCTION(BlueprintCallable)
	void AddAmmo(int addAmmo, EAmmoType ammoType);
	// 5.z PICKUP

public:
	bool bIsFireRatePassed = true;

	

protected:

	
	/**
	*if target duration is below 0, 
	*it will return 1 playrate
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	float GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage);

	

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
	/**ammo is 0*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponRunOutOfAmmo();
	bool CheckAndCallRunOutOfAmmo(int ammo);
	/**energy is 0*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponRunOutOfEnergy();
	/**it is overheating*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponIsOverheating();
	/**
	* also use this for 
	* interface to anim BP
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (KeyWords = "change interface"))
	void OnSwitchWeaponSuccess();

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////E-z
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsFireRatePassed();
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void SetIsFireRatePassed(const bool bFireRatePassed);
	

	// a Overriden Function
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsSwitchWeaponRequirementFulfilled();
	bool IsSwitchWeaponRequirementFulfilled_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToRepeatAutoFire();
	bool IsAbleToRepeatAutoFire_Implementation();
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Fire")
	bool IsCharacterAbleToFire();
	bool IsCharacterAbleToFire_Implementation();
	// z Overriden Function

	bool IsWeaponAbleToFire();
	bool IsEnoughForWeaponCost();
	bool IsAmmoEnough(EAmmoType ammo);
	bool IsNotOverheat();
	float WeaponTemperature;
	bool IsEnergyEnough();
	void TimerFireRate_Start();
	FTimerHandle FireRateTimer;
	void TimerFireRate_Reset();
	

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNames;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

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
	float ProjectileMultiplier = 1.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRightAtRate(float Rate);
	void LookUpAtRate(float Rate);

	bool bIsTriggerPressed;

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool isTransitioningAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming();

	bool bIsTransitioningAiming;

private:


	void SetWeaponIndexWithNumpad(const int numberInput);
	void SetWeaponIndexWithNumpad_1();
	void SetWeaponIndexWithNumpad_2();
	void SetWeaponIndexWithNumpad_3();
	void SetWeaponIndexWithNumpad_4();

	void SetWeaponIndexWithMouseWheel(const bool isUp);
	void SetWeaponIndexWithMouseWheel_Up();
	void SetWeaponIndexWithMouseWheel_Down();

	const float BaseTurnRate = 45.0f;
	const float BaseLookUpRate = 45.0f;
	float NormalizedForward;
	float NormalizedRight;

	bool bIsAiming;

	void OrientCharacter(const bool bMyCharIsAiming);
	
	bool bForwardInputPressed;

	bool bRightInputPressed;

	float AssignNormalizedVelo(float MyValue,  bool bOtherButtonPressed);

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class UTPS_Weapon* GetRangedWeapon() const { return RangedWeapon; }
};