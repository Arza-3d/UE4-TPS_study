// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ConstructorHelpers.h"
#include "Particles/ParticleSystem.h"
#include "Custom/ShouldCheckFile.h"
#include "ProjectileStruct.generated.h"

class USoundBase;
class UProjectileParticleDataAsset;
class UProjectileSoundDataAsset;
//class UParticleSystem;

USTRUCT(BlueprintType)
struct FProjectileData
{
	GENERATED_BODY();

	/**
	 * 0 = speed
	 * 1 = gravity scale
	 * 2 = particle scale
	 */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<float> SpeedxGravityxScale;
};

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
	TArray<USoundBase*> HitAndTrailSound = { nullptr };

	FProjectileSound()
	{
		if (SHOULDNOTCHECKFILEFORSTRUCT) return;

		if (MuzzleSound == nullptr)
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> inSound1(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_Shot_Stereo_Cue.AssaultRifle_Shot_Stereo_Cue'"));
			if (SHOULDCHECKFILE) check(inSound1.Succeeded());
			MuzzleSound = inSound1.Object;
		}

		if (HitAndTrailSound[0] == nullptr)
		{
			static ConstructorHelpers::FObjectFinder<USoundBase> inSound2(TEXT("SoundCue'/Game/Sounds/Weapon_AssaultRifle/Stereo/AssaultRifle_End_Stereo_Cue.AssaultRifle_End_Stereo_Cue'"));
			if (SHOULDCHECKFILE) check(inSound2.Succeeded());
			HitAndTrailSound[0] = inSound2.Object;
		}

	}
};

USTRUCT(BlueprintType)
struct FProjectileParticle
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> MuzzleParticle = { nullptr };

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> TrailParticle = { nullptr };

	/**
	 * 0 = HitWorld
	 * 1 = HitCharacter
	 * 2 = HiWater
	 * 3 = NoHit
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> HitParticle;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TArray<UParticleSystem*> OtherParticle;

	FProjectileParticle()
	{
		if (SHOULDNOTCHECKFILEFORSTRUCT) return;

		if (MuzzleParticle[0] == nullptr)
		{
			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
			if (SHOULDCHECKFILE) check(inParticle.Succeeded());
			MuzzleParticle[0] = inParticle.Object;
		}

		if (TrailParticle[0] == nullptr)
		{
			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle2(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_Trail.P_Wraith_Primary_Trail'"));
			if (SHOULDCHECKFILE) check(inParticle2.Succeeded());
			TrailParticle[0] = inParticle2.Object;
		}

		HitParticle.SetNum(1);

		if (HitParticle[0] == nullptr)
		{
			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle3(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld.P_Wraith_Primary_HitWorld'"));
			if (SHOULDCHECKFILE) check(inParticle3.Succeeded());
			HitParticle[0] = inParticle3.Object;

			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle4(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitCharacter.P_Wraith_Primary_HitCharacter'"));
			if (SHOULDCHECKFILE) check(inParticle4.Succeeded());
			HitParticle.Add(inParticle4.Object);

			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle5(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld_Water.P_Wraith_Primary_HitWorld_Water'"));
			if (SHOULDCHECKFILE) check(inParticle5.Succeeded());
			HitParticle.Add(inParticle5.Object);

			static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle6(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_NoHit.P_Wraith_Primary_NoHit'"));
			if (SHOULDCHECKFILE) check(inParticle6.Succeeded());
			HitParticle.Add(inParticle6.Object);
		}
	}
};

USTRUCT(BlueprintType)
struct FProjectile
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FProjectileData ProjectileData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileParticleDataAsset* ProjectileParticle;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UProjectileSoundDataAsset* ProjectileSound;
};

UCLASS()
class TPS_STUDY_API UProjectileStruct : public UObject
{
	GENERATED_BODY()
	
};
