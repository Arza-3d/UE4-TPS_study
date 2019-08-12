// Arza.3d

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TPS_FunctionLibrary.h"
//#include "Components/TimelineComponent.h"
#include "TPS_studyCharacter.generated.h"

/*UENUM(BlueprintType)
enum class ECharacterLocomotionState : uint8
{
	Idle,
	Jog,
	Run,
	Sprint,
	Ragdoll
};*/

UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UTPS_Weapon* RangedWeapon;

public:

	ATPS_studyCharacter();

	bool bIsFireRatePassed;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Ammo")
	FAmmoCount Ammunition;

	UFUNCTION(BlueprintCallable)
	void AddAmmo(int addAmmo, EAmmoType ammoType);

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

	void CharacterPlayMontage();
	

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming", meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed();

	/**
	*if target duration is below 0, 
	*it will return 1 playrate
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	float GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage);

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

	////////////////////////////////////////////////////////////////////////////////////////////BPN-a

	bool CanWeaponFire();
	
	bool IsEnoughForWeaponCost();
	bool IsAmmoEnough(EAmmoType ammo);
	bool IsNotOverheat();
	float WeaponTemperature;
	bool IsEnergyEnough();
	void StartFireRateCount();
	FTimerHandle FireRateTimer;
	void ResetFireRateCount();
	

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

	float BaseTurnRate;

	float BaseLookUpRate;
	
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