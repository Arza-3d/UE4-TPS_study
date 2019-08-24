#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "TPSFunctionLibrary.h"
#include "TPS_studyCharacter.generated.h"


UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter {
	GENERATED_BODY()

public:

	ATPS_studyCharacter();

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetIsAiming();

	float GetHP() const;

	void SetHP(float val);

	float GetMP() const;

	void SetMP(float val);

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward() const;

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight() const;

protected:

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	/** Only use this for stationary/vehicle/drone weapon */
	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	bool bWeaponIsAlwaysAiming;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<class UUserWidget> Crosshair;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	ETriggerMechanism GetTriggerMechanism() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FName GetWeaponName() const;

	/**
	* 0 = default
	* 1 = aiming
	* 2, 3, x extra mode
	*/
	TArray<FAimingStat> AimStats;

	int AimStatStartIndex = 0;
	int AimStatTargetIndex = 1;
	TArray<FName> AimingNames;

	void Aiming();
	void AimingStop();
	void AimingSetup(const bool isAiming);
	
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

	// 3.a FIRE

protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed() const;
	
	bool bIsFireRatePassed = true;
	
	FRotator GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform);

	/** This will be connected to controller to fire action input press*/
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FirePress();

	/** This will be connected to controller to fire action input releases*/
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();
	
	// 3.z FIRE

public:

	FShooter ShooterState;

protected:

	FAmmoCount Ammunition;

	FExternalEnergyCount Energy;

	FWeapon CurrentWeapon;

	FProjectile CurrentProjectile;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* WeaponModeTable;

	/**
	* FAimingStatCompact
	* contain aiming stat related
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	class UDataTable* AimingTable;

	

	///////////////////////
	// 4.a SWITCH WEAPON
	///////////////////////

	

	// 5.a PICKUP
public:

	UFUNCTION(BlueprintCallable)
	void AddAmmo(int addAmmo, EAmmoType ammoType);

protected:

	////////////
	// a Event
	////////////

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Aiming")
	void OnAiming();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Aiming")
	void OnStopAiming();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponFires();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoAmmo();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoEnergy();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnWeaponOverheats();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnNoMoreAmmoDuringFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (KeyWords = "change interface"))
	void OnSwitchWeaponSuccess();

	/////////////////////////
	// overriden function:
	////////////////////////

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
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNames;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetWeaponIndex() const; 

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetLastWeaponIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool isTransitioningAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	class ATPS_Projectile* TheProjectile;

	FCharacterStat CharacterStat;
	FExternalEnergyCount EnergyExternal;

	void FireStandardTrigger();
	void FireAutomaticTrigger();
	void FireHold();
	void FireAutomaticTriggerOnePress();

	void FireReleaseAfterHold();

	bool IsWeaponAbleToFire();

	void FireProjectile();
	void FireProjectile(EAmmoType AmmoType);
	void FireProjectile(EEnergyType EnergyType);
	void FireProjectile(int* Ammo);
	void FireProjectile(float* Energy);

	void SpawnProjectile(USceneComponent* WeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int i);

	FTimerHandle FireRateTimer;
	void TimerFireRateStart();
	void TimerFireRateReset();

	void PlayFireMontage();

	USceneComponent* WeaponInWorld;

	void SwitchWeaponMesh();

	void SetWeaponMode(int MyWeaponIndex);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRightAtRate(float Rate);
	void LookUpAtRate(float Rate);

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType ammo);
	bool IsAmmoEnough(const EEnergyType EnergyType);
	bool IsAmmoEnough(const int ammo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();

	int WeaponIndex;
	int LastWeaponIndex;

	bool bIsTriggerPressed;
	bool bIsAiming;
	bool bIsTransitioningAiming;
	bool bOnePressToggle;

	bool bForwardInputPressed;
	bool bRightInputPressed;

	void FlipOnePressTriggerSwitch();

	void SetWeaponIndex(const int NumberInput);
	void SetWeaponIndex1();
	void SetWeaponIndex2();
	void SetWeaponIndex3();
	void SetWeaponIndex4();

	void SetWeaponIndex(const bool isUp);
	void SetWeaponIndexUp();
	void SetWeaponIndexDown();

	const float BaseTurnRate = 45.0f;
	const float BaseLookUpRate = 45.0f;

	float NormalizedForward;
	float NormalizedRight;

	void OrientCharacter(const bool bMyCharIsAiming);
	
	float AssignNormalizedVelo(float MyValue,  bool bOtherButtonPressed);

};