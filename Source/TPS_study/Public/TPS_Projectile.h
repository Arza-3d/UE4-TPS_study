// Arza.3d
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TPSFunctionLibrary.h"
#include "TPS_Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UParticleSystemComponent;
class UPrimitiveComponent;


UCLASS()
class TPS_STUDY_API ATPS_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	ATPS_Projectile();

	FProjectileMuzzle ProjectileMuzzle;
	FProjectileTrail ProjectileTrail;
	FProjectileHit ProjectileHit;
	UProjectileFXDataAsset* ProjectileVX;
	UProjectileSXDataAsset* ProjectileSound;

	void SetUpProjectile(FProjectile MyProjectile);

	void SpawnFX(TArray<UParticleSystem*> MyParticle, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter);

	void SpawnFX(UParticleSystem* MyParticle, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter);

	void DestroySelf();

	virtual void NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	UFUNCTION()
	void ShowOverlapObjectData(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	//FString GetCollisionEnumAsString(ECollisionChannel EnumValue);

	UFUNCTION()
	void ShowHitObjectData(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	virtual void BeginPlay() override;

private:
	FTimerHandle TimerDestroy;
	
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	UParticleSystemComponent* ProjectileTrailParticle;
};
