#include "TPSFunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "Curves/CurveFloat.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

/**
*if target duration is below 0,
*it will return 1 playrate
*/
float UTPSFunctionLibrary::GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage)
{
	return (targetDuration <= 0.0f) ? 1.0f : animMontage->SequenceLength / targetDuration;
}

UParticleSystem* UTPSFunctionLibrary::GetRandomParticle(TArray<UParticleSystem*> particleSystems)
{
	int maxInt = particleSystems.Num();
	int randInt = UKismetMathLibrary::RandomInteger(maxInt);

	return particleSystems[randInt];
}

float UTPSFunctionLibrary::StandardLinearInterpolation(const float X, const float X1, const float X2, const float Y1, const float Y2)
{
	const float gradient = (Y2 -Y1) / (X2 - X1);

	return gradient * (X - X1) + Y1;
}