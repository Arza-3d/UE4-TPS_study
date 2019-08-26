#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSFunctionLibrary.h"
#include "RangedWeaponComponent.generated.h"

class UDataTable;
class UUserWidget;
class ATPShooterCharacter;
class ATPS_studyCharacter;

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

	//=================
	// Getter (public):
	//=================

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

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void AddAmmo(const int32 addAmmo, const EAmmoType ammoType);

	UFUNCTION(BlueprintCallable, Category ="Fire")
	void FirePress();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireRelease();

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<UUserWidget> Crosshair;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	UDataTable* AimingTable;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UDataTable* WeaponTable;
	
//===========================================================================
private:
//===========================================================================

	//========================
	// Shotter stat (private):
	//========================

	ATPShooterCharacter* Shooter;

	//=======================
	// Weapon stat (private):
	//=======================

	FAmmoCount Ammunition;
	FExternalEnergyCount EnergyExternal;

	FWeapon CurrentWeapon;
	FProjectile CurrentProjectile;

	int32 WeaponIndex;
	int32 LastWeaponIndex;
	TArray<FName> WeaponNames;
	
	//==================
	// Aiming (private):
	//==================

	bool bMaxHoldIsReach;
	float MaxFireHoldTime;

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
