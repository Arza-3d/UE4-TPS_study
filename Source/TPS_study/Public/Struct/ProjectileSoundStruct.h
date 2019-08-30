// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ConstructorHelpers.h"
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
		static ConstructorHelpers::FObjectFinder<USoundBase> inSound(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_Shot_Stereo_Cue.AssaultRifle_Shot_Stereo_Cue'"));
		check(inSound.Succeeded());

		MuzzleSound = inSound.Object;

		static ConstructorHelpers::FObjectFinder<USoundBase> inMuzzleSound(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_End_Stereo_Cue.AssaultRifle_End_Stereo_Cue'"));
		check(inSound.Succeeded());

		HitAndTrailSound[0] = inSound.Object;
	}
};

UCLASS()
class TPS_STUDY_API UProjectileSoundStruct : public UObject
{
	GENERATED_BODY()
	
};
