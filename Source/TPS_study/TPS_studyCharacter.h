#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "TPS_FunctionLibrary.h"
#include "TPS_studyCharacter.generated.h"

UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter {
	GENERATED_BODY()

	///////////////////
	// 0.Construction
	///////////////////

public:
	ATPS_studyCharacter();
	
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RangedWeapon", meta = (AllowPrivateAccess = "true"))
	class UTPS_Weapon* RangedWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class ATPS_Projectile> TheProjectile;
	
	FCharacterStat CharacterStat;
	FExternalEnergyCount EnergyExternal;

public:

	float GetHP() const;

	void SetHP(float val);

	float GetMP() const;

	void SetMP(float val);

protected:

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void Tick(float DeltaSeconds) override;

	/////////////////
	// 1.Navigation
	////////////////

public:

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward();

	/**only used for aim anim blend walk*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight();

	////////////
	// 2.Aiming
	////////////

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<class UUserWidget> Crosshair;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	ETriggerMechanism GetTriggerMechanism() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FName GetWeaponName();

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
	void Aiming_Setup(const bool isAiming);
	
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming", meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed() const;
	
	void SetWeaponMode(int MyWeaponIndex);

	bool IsThereStillAmmoLeft();
	FRotator GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform);

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FirePress();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();

	void FireUnlimited();
	void FireAmmo();
	void FireEnergy();
	void FireAmmoProjectile(int* Ammo);
	void FireEnergyProjectile(float* Energy);
	void FireStandardTrigger();
	void SpawnProjectile(USceneComponent* WeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int i);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnRunOutOfAmmoDuringMultipleFire();

	void FireAutomaticTrigger();
	void Fire_Hold();
	void Fire_Release();
	void FireAutomaticTriggerOnePress();
	void PlayFireMontage();

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

	// 4.a SWITCH WEAPON
	USceneComponent* WeaponInWorld;
	void SwitchWeaponMesh();

	// 5.a PICKUP
public:
	UFUNCTION(BlueprintCallable)
	void AddAmmo(int addAmmo, EAmmoType ammoType);

	////////////////
	// 5.z PICKUP
	///////////////
public:
	bool bIsFireRatePassed = true;

protected:

	// a EVENT
	/**on aiming succeed*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Aiming")
	void OnAimingSucceed();

	/**for press trigger*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	void OnFire();

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

	/**also use this for interface to anim BP*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (KeyWords = "change interface"))
	void OnSwitchWeaponSuccess();
	// z Event

	// a Overriden Function
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
	// z Overriden Function

	bool IsEnoughForWeaponCost();
	bool IsAmmoEnough(EAmmoType ammo);
	bool IsEnergyEnough(EEnergyType EnergyType);
	FTimerHandle FireRateTimer;
	void TimerFireRateStart();
	void TimerFireRateReset();
	
	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNames;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetWeaponIndex() const; 
	int WeaponIndex;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetLastWeaponIndex() const;
	int LastWeaponIndex;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRightAtRate(float Rate);
	void LookUpAtRate(float Rate);
	bool bIsTriggerPressed;

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool isTransitioningAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

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