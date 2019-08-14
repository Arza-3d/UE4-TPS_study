// Fill out your copyright notice in the Description page of Project Settings.


#include "TPS_FunctionLibrary.h"
#include "Particles/ParticleSystem.h"

UParticleSystem* UTPS_FunctionLibrary::GetRandomParticle(TArray<UParticleSystem*> particleSystems)
{
	int maxInt = particleSystems.Num();
	int randInt = UKismetMathLibrary::RandomInteger(maxInt);
	return particleSystems[randInt];
}
