// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ConstructorHelpers.h"
//#include "Sound/SoundBase.h"

#include "Custom/ShouldCheckFile.h"
#include "Library/TPSFunctionLibrary.h"
#include "ProjectileSoundStruct.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FProjectileSound
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	USoundBase* MuzzleSound;

	/**
	 * 0 = hit sound
	 * 1 = trail sound
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<USoundBase*> HitAndTrailSound = {nullptr};

	FProjectileSound()
	{
		//UTPSFunctionLibrary::GetThisObject<USoundBase>(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_Shot_Stereo_Cue.AssaultRifle_Shot_Stereo_Cue'"));

		static ConstructorHelpers::FObjectFinder<USoundBase> inSound1(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_Shot_Stereo_Cue.AssaultRifle_Shot_Stereo_Cue'"));
		if(SHOULDCHECKFILE) check(inSound1.Succeeded());
		MuzzleSound = inSound1.Object;

		static ConstructorHelpers::FObjectFinder<USoundBase> inSound2(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_End_Stereo_Cue.AssaultRifle_End_Stereo_Cue'"));
		if(SHOULDCHECKFILE) check(inSound2.Succeeded());
		HitAndTrailSound[0] = inSound2.Object;
	}
};

UCLASS()
class TPS_STUDY_API UProjectileSoundStruct : public UObject
{
	GENERATED_BODY()
	
};
