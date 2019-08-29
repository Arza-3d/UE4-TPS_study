#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSFunctionLibrary.h"
#include "RangedWeaponComponent.generated.h"

class UDataTable;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchWeapon, URangedWeaponComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFireSignature, URangedWeaponComponent*, MyComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnMaxFireHoldRelease, URangedWeaponComponent*, MyComponent, const float, MyCurrentHoldTime, const float, MyMaxHoldTime);

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

//===========================================================================
public:	
//===========================================================================

	URangedWeaponComponent();

	//================
	// Event (public):
	//================

	/** Called when actor fire the weapon */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnFireSignature OnFire;

	/** Called when max fire hold is reach */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnJustReachMaxFireHoldSignature OnJustReachMaxHoldTrigger;

	/** Called when actor can't shoot more during multiple shot  */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnMaxFireHoldRelease OnMaxFireHoldRelease;

	/** Called when actor switch weapon */
	UPROPERTY(BlueprintAssignable, Category = "Switch Weapon Event")
	FOnSwitchWeapon OnSwitchWeapon;

	//=================
	// Getter (public):
	//=================

	//-------------------------------------------------------------

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

	//==================================
	// Function for Controller (public):
	//==================================

	UFUNCTION(BlueprintCallable, Category ="Fire")
	void FirePress();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	//===============================
	// Default Variables (protected):
	//===============================

	UPROPERTY(EditDefaultsOnly, Category = "Weapon", Meta = (PriorityOrder = "1"))
	UDataTable* WeaponTable;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	bool bIsAbleToShootWithoutAiming;
	
//===========================================================================
private:
//===========================================================================

	//==================
	// Getter (private):
	//==================
	APawn* GetPawn();

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
	
	//================
	// Fire (private):
	//================

	bool bOnePressToggle;
	bool bMaxHoldIsReach;
	bool bIsTriggerPressed;
	bool bIsFireRatePassed = true;

	float MaxFireHoldTime;

	USceneComponent* WeaponInWorld;
	void SetWeaponMesh();

	void SetWeaponMode(const int32 MyWeaponIndex);
	void SetWeaponIndex(const int32 InNumber);
	void SetWeaponIndex(const bool isUp);

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
