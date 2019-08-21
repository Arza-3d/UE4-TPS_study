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
	ATPS_Projectile();
	FProjectileMuzzle ProjectileMuzzle;
	FProjectileTrail ProjectileTrail;
	FProjectileHit ProjectileHit;
	/*void SetProjectileMuzzle();
	void SetProjectileTrail();
	void SetProjectileHit();*/
	void SetUpProjectile(FProjectile MyProjectile);
	void PlayFX(TArray<UParticleSystem*> MyParticle, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter);
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void OnProjectileOverlaped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	void DestroySelf();

	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
protected:
	virtual void BeginPlay() override;

private:
	FTimerHandle TimerDestroy;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ParticleComp;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UStaticMesh* ProjectileMeshTest;

};
