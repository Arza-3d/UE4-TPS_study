// Arza.3d
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPS_FunctionLibrary.h"
#include "TPS_Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystemComponent;

UCLASS()
class TPS_STUDY_API ATPS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATPS_Projectile(/*FProjectile projectile*/);
	
	FProjectileMuzzle ProjectileMuzzle;
	FProjectileTrail ProjectileTrail;
	FProjectileHit ProjectileHit;

	/*void SetProjectileMuzzle();
	void SetProjectileTrail();
	void SetProjectileHit();*/

	void SetUpProjectile(FProjectile myProjectile);

private:

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;

};
