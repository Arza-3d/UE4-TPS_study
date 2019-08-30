// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Particles/ParticleSystem.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectileParticleStruct.generated.h"


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

	FProjectileParticle()
	{
		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_MuzzleFlash.P_Wraith_Primary_MuzzleFlash'"));
		check(inParticle.Succeeded());
		MuzzleParticle[0] = inParticle.Object;

		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle2(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_Trail.P_Wraith_Primary_Trail'"));
		check(inParticle2.Succeeded());
		TrailParticle[0] = inParticle2.Object;

		HitParticle.SetNum(4);

		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle3(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld.P_Wraith_Primary_HitWorld'"));
		check(inParticle3.Succeeded());
		HitParticle[0] = inParticle3.Object;

		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle4(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitCharacter.P_Wraith_Primary_HitCharacter'"));
		check(inParticle4.Succeeded());
		HitParticle[1] = inParticle4.Object;

		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle5(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_HitWorld_Water.P_Wraith_Primary_HitWorld_Water'"));
		check(inParticle5.Succeeded());
		HitParticle[2] = inParticle5.Object;

		static ConstructorHelpers::FObjectFinder<UParticleSystem> inParticle6(TEXT("ParticleSystem'/Game/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_NoHit.P_Wraith_Primary_NoHit'"));
		check(inParticle6.Succeeded());
		HitParticle[3] = inParticle6.Object;
	}
};


UCLASS()
class TPS_STUDY_API UProjectileParticleStruct : public UObject
{
	GENERATED_BODY()
	
};
