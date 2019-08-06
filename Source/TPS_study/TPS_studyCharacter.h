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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float NormalizedForward;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation")
	float NormalizedRight;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Aiming")
	bool bIsAiming;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Fire")
	bool bIsTriggerPressed;

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void OrientCharacter(bool bMyCharIsAiming);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire",
		meta = (ToolTip = "repeat fire for automatic weapon"))
	void RepeatFire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Fire",
		meta = (ToolTip = "regular weapon fire"))
	void FireCPP();


protected:

	void MoveForward(float Value);

	void MoveRight(float Value);

	void TurnAtRate(float Rate);

	void LookUpAtRate(float Rate);

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//UPROPERTY(VisibleAnywhere, Category = Aiming)
	//class UTimelineComponent* AimingTransitionTimeline;

private:

	bool bForwardInputPressed;

	bool bRightInputPressed;

	float AssignNormalizedVelo(float MyValue,  bool bOtherButtonPressed);

public:

	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};