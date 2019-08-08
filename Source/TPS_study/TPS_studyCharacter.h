#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
//#include "Components/TimelineComponent.h"
#include "TPS_studyCharacter.generated.h"

//lass UTimeLineComponent

UCLASS(config=Game)
class ATPS_studyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	//class UTimeLineComponent* CPPAimingTimeline;

public:

	ATPS_studyCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	float BaseLookUpRate;

	bool bIsFireRatePassed;
	
protected:

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming",
		meta = (ToolTip = "is character aiming?"))
	bool GetIsAiming();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire")
	bool GetIsTriggerPressed();

	UFUNCTION(BlueprintCallable, Category = "Fire")
	void SetIsTriggerPressed(bool bTriggerPressed);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation", meta = (ToolTip = "only used for aim anim blend walk"))
	float GetNormalizedForward();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Animation", meta = (ToolTip = "only used for aim anim blend walk"))
	float GetNormalizedRight();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	class UDataTable* WeaponTableCPP;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (ToolTip = "repeat fire for automatic weapon"))
	void RepeatFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire", meta = (ToolTip = "regular weapon fire"))
	void Fire();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	bool GetIsFireRatePassed();

	UFUNCTION(BlueprintCallable, Category = "Fire", meta = (ToolTip = "true after fire rate is passed"))
	void SetIsFireRatePassed(bool bFireRatePassed);

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingCPP(bool bIsCharAiming);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	bool IsWeaponNameInThisIndexExist(int weaponIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent, Category = "Weapon")
	bool IsSwitchWeaponRequirementFulfilled();

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
	TArray<FName> WeaponNamesCPP;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponIndexCPP(int weaponIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int GetWeaponIndexCPP();

	int WeaponIndexCPP;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

	bool bIsTriggerPressed;

	

	//UPROPERTY(VisibleAnywhere, Category = Aiming)
	//class UTimelineComponent* AimingTransitionTimeline;

private:

	
	
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