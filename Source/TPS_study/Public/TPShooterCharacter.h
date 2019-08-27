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
class UHPandMPComponent;
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

	FORCEINLINE UHPandMPComponent* GetHealthAndMana() const { return HealthAndMana; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetIsAiming() const;

	float GetHP() const;

	void SetHP(float val);

	float GetMP() const;

	void SetMP(float val);

	//===========================
	// TPS Anim Interface Getter:
	//===========================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Interface")
	UAnimInstance* GetAnimBlueprint() const;

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
	bool bIsAbleToShootWithoutAiming;

	//====================
	// Getter (protected)
	//====================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

	//======================
	// Function (protected)
	//======================

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

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnWeaponFires();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnMaxFireHoldRelease();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnMaxFireHold();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnNoEnergy();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnWeaponOverheats();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire")
	//void OnNoMoreAmmoDuringFire();

	//UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (KeyWords = "change interface"))
	//void OnSwitchWeapon();

	//================================
	// overriden function (protected)
	//================================

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsAbleToSwitchWeapon();
	virtual bool IsAbleToSwitchWeapon_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Aiming")
	bool IsAbleToAim();
	virtual bool IsAbleToAim_Implementation();

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UHPandMPComponent* HealthAndMana;

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

	void Aiming(const bool bInIsAiming);
	void OrientCharacter(const bool bMyCharIsAiming);

	FCharacterStatBPCPP CharacterStat;
	FShooter ShooterState;

	//=============
	// Fire Weapon:
	//=============

	void FirePress();
	void FireRelease();

	//===============
	// Switch Weapon:
	//===============

	void SetWeaponIndex1();
	void SetWeaponIndex2();
	void SetWeaponIndex3();
	void SetWeaponIndex4();

	void SetWeaponIndexUp();
	void SetWeaponIndexDown();

	//============
	// Navigation:
	//============

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