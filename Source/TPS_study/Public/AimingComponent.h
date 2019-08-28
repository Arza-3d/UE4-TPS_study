// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AimingComponent.generated.h"


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
	
};
