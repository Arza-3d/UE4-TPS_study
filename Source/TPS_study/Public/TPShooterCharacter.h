#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "HPandMPComponent.h"
#include "TPSFunctionLibrary.h"
#include "TPShooterCharacter.generated.h"

class UAimingComponent;
class UDataTable;
class UUserWidget;
class USpringArmComponent;
class UCameraComponent;
class UHPandMPComponent;
class URangedWeaponComponent;
class ATPS_Projectile;
class UAmmoAndEnergyComponent;

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

	FORCEINLINE UAimingComponent* GetAiming() const { return Aiming; }

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//====================
	// Getter (protected):
	//====================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedForward() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation")
	float GetNormalizedRight() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation Interface")
	UAnimInstance* GetAnimBlueprint() const;

	//================================
	// Overriden function (protected):
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

//===========================================================================
private:
//===========================================================================
	
	FCharacterStatBPCPP CharacterStat; // will delete later
	//struct FShooter ShooterState;

	//===============================
	// Blueprint Component (private):
	//===============================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UHPandMPComponent* HealthAndMana;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	URangedWeaponComponent* RangedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UAimingComponent* Aiming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UAmmoAndEnergyComponent* Ammo;

	//==================
	// Aiming (private):
	//==================

	void AimingPress();
	void AimingRelease();

	//=======================
	// Fire Weapon (private):
	//=======================

	void FirePress();
	void FireRelease();

	//=========================
	// Switch Weapon (private):
	//=========================

	void SetWeaponIndex1();
	void SetWeaponIndex2();
	void SetWeaponIndex3();
	void SetWeaponIndex4();

	void SetWeaponIndexUp();
	void SetWeaponIndexDown();

	//======================
	// Navigation (private):
	//======================

	const float BaseTurnRate = 45.0f;
	const float BaseLookUpRate = 45.0f;

	bool bForwardInputPressed;
	bool bRightInputPressed;

	float NormalizedForward;
	float NormalizedRight;

	//-------------------------------------------------------------

	float AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed);

	void MoveForward(float Value);
	void MoveRight(float Value);
	void LookRightAtRate(float Rate);
	void LookUpAtRate(float Rate);
};