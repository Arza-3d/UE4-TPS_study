#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
// custom:
#include "Enum/AimingEnum.h"
#include "Struct/AimingStruct.h"
//
#include "AimingComponent.generated.h"

class USpringArmComponent;
class UUserWidget;
class ACharacter;
class UCameraComponent;
class UCharacterMovementComponent;
class UCurveFloat;
class UDataTable;

//=================
// Aiming DELEGATE:
//=================

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAiming, UAimingComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTransitioningAiming, UAimingComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStopAiming, UAimingComponent*, MyComponent);

//=======================
// AimingComponent CLASS:
//=======================

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UAimingComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class URangedWeaponComponent;

//===========================================================================
public:
//===========================================================================

	UAimingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//================
	// Event (public):
	//================

	/** Called when actor reach aiming state */
	UPROPERTY(BlueprintAssignable)
	FOnAiming OnAiming;

	/** Called when actor in transitioning in aiming state */
	UPROPERTY(BlueprintAssignable)
	FOnTransitioningAiming OnTransitioningAiming;

	/** Called when actor reach aiming state */
	UPROPERTY(BlueprintAssignable)
	FOnStopAiming OnStopAiming;

	//=================
	// Getter (public):
	//=================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetIsAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Aiming")
	bool GetTransitioningAiming() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	float GetAimingAlpha() const;

	//=================
	// Setter (public):
	//=================

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void SetIsTransitioningAiming(bool bInBool);

	//==================================
	// Function for Controller (public):
	//==================================

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingPress();

	UFUNCTION(BlueprintCallable, Category = "Aiming")
	void AimingRelease();

//===========================================================================
protected:
//===========================================================================
	
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	float TotalAimingTime = 0.8f;// delete later

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	UDataTable* AimingTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<UUserWidget> Crosshair;

	/**
	 * Curve time must be 0 to 1
	 * Curve value muse be 0 to 1
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Aiming", Meta = (DisplayName = "Aiming Curve 0 -> 1", DisplayPriority = "1"))
	UCurveFloat* AimingCurve;

//===========================================================================
private:
//===========================================================================

	template <class ThisType>
	ThisType* GetThisType() const
	{
		TArray<UActorComponent*> myComponents = GetOwner()->GetComponents().Array();
		ThisType* returnedVal = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			returnedVal = Cast<ThisType>(myComponents[i]);
			if (returnedVal) break;
		}
		return returnedVal;
	}

	//==================
	// Getter (private):
	//==================

	ACharacter* GetCharacter();
	UCharacterMovementComponent* GetCharacterMovement();

	UCameraComponent* CameraComponent;
	USpringArmComponent* CameraBoomComponent;
	URangedWeaponComponent* RangedWeaponComponent;

	bool bIsAimingForward;

	float DeltaSecond;
	float AimingAlpha;
	float CurrentAimingTime;

	int32 AimStatStartIndex;
	int32 AimStatTargetIndex = 1;

	EAimingState AimingState;

	TArray<FName> AimingNames;
	TArray<FAimingStat> AimStats; // 0 = default, 1 = aiming, 2, 3, x extra mode

	FTimerHandle AimingTimerHandle;

	void AimingTimerStart();
	void ClearAndStartAimingTimer();
	void OrientCharacter(const bool bMyCharIsAiming);
	void ClearAndInvalidateAimingTimer(const float NewCurrentTime);
	void TimeAiming(float InAlpha);
	
};
