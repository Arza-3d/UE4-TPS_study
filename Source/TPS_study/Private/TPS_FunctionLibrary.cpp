#include "TPS_FunctionLibrary.h"
#include "Animation/AnimMontage.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"

/**
*if target duration is below 0,
*it will return 1 playrate
*/
float UTPS_FunctionLibrary::GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage) {
	return (targetDuration <= 0.0f) ? 1.0f : animMontage->SequenceLength / targetDuration;
}
UParticleSystem* UTPS_FunctionLibrary::GetRandomParticle(TArray<UParticleSystem*> particleSystems) {
	int maxInt = particleSystems.Num();
	int randInt = UKismetMathLibrary::RandomInteger(maxInt);
	return particleSystems[randInt];
}
