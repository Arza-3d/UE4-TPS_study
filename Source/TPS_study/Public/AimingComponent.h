// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimingComponent.generated.h"

/*UENUM(BlueprintType)
enum class EAimingState : uint8
{
	NotAiming,
	TransitioningAiming,
	Aiming
};*/

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UAimingComponent : public UActorComponent
{
	GENERATED_BODY()

//===========================================================================
public:
//===========================================================================

	UAimingComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//===========================================================================
protected:
//===========================================================================
	
	virtual void BeginPlay() override;
	/*
	//==================
	// Aiming (private):
	//==================

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
	*/
	
};
