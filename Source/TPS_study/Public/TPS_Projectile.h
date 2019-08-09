// Arza.3d
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPS_Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystemComponent;

UCLASS()
class TPS_STUDY_API ATPS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATPS_Projectile();

private:

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;

};
