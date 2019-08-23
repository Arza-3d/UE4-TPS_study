#include "TPS_Projectile.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectileFXDataAsset.h"
#include "ProjectileSXDataAsset.h"
#include "TimerManager.h"
#include "TPSFunctionLibrary.h"

ATPS_Projectile::ATPS_Projectile() 
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	ProjectileTrailParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ProjectileTrailParticle"));
	ProjectileTrailParticle->Activate(true);
	CollisionComp->bHiddenInGame = false;
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionObjectType(ProjectileMuzzle.CollisionComp);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	RootComponent = CollisionComp;
	ProjectileTrailParticle->SetupAttachment(CollisionComp);
}

void ATPS_Projectile::SetUpProjectile(FProjectile MyProjectile) 
{
	ProjectileMuzzle = MyProjectile.Muzzle;
	ProjectileTrail = MyProjectile.Trail;
	ProjectileHit = MyProjectile.Hit;
	ProjectileVX = MyProjectile.ProjectileVX;
	ProjectileSound = MyProjectile.ProjectileSoundEffect;

	MovementComp->InitialSpeed = ProjectileMuzzle.InitialSpeedAndOther[0];
	MovementComp->ProjectileGravityScale = (ProjectileMuzzle.InitialSpeedAndOther.Num() > 2) ? ProjectileMuzzle.InitialSpeedAndOther[2] : 0.0f;	
}

void ATPS_Projectile::BeginPlay() 
{
	Super::BeginPlay();
	
	if (ProjectileVX) {
		SpawnFX(
			ProjectileVX->ProjectileVX.MuzzleVX,//ProjectileMuzzle.MuzzleFX.VisualEffect, 
			ProjectileSound->ProjectileSoundEffect.MuzzleSX,//ProjectileMuzzle.MuzzleFX.SoundEffect,
			GetActorTransform(), 1.0f
		);

		ProjectileTrailParticle->SetTemplate(UTPSFunctionLibrary::GetRandomParticle(
			ProjectileVX->ProjectileVX.TrailVX//ProjectileTrail.TrailFX.VisualEffect
		));
	}
}

void ATPS_Projectile::DestroySelf() { GetWorld()->DestroyActor(this); }

void ATPS_Projectile::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) 
{
	if (ProjectileVX && ProjectileSound) {
		SpawnFX(
			ProjectileVX->ProjectileVX.HitVX, //ProjectileHit.HitFX[0].VisualEffect
			ProjectileSound->ProjectileSoundEffect.HitSX,//ProjectileHit.HitFX[0].SoundEffect, 
			GetActorTransform(), 1.0f
		);
	}
	if (ProjectileTrailParticle) {
		ProjectileTrailParticle->DestroyComponent();
	}
	GetWorldTimerManager().ClearTimer(TimerDestroy);
	GetWorldTimerManager().SetTimer(TimerDestroy, this, &ATPS_Projectile::DestroySelf, 3.0f);
}

void ATPS_Projectile::SpawnFX(TArray<UParticleSystem*> MyParticles, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter) 
{
	if (MyParticles.Num() > 0) 
	{
		UParticleSystem* MyParticle = UTPSFunctionLibrary::GetRandomParticle(MyParticles);
		UGameplayStatics::SpawnEmitterAtLocation(this, MyParticle, MyTransform.GetLocation(), MyTransform.Rotator(), MyTransform.GetScale3D(), false, EPSCPoolMethod::None);
	}
	
	if (MySoundEffect != nullptr) 
	{
		UGameplayStatics::PlaySoundAtLocation(this, MySoundEffect, MyTransform.GetLocation());
	}
}

void ATPS_Projectile::SpawnFX(UParticleSystem * MyParticle, USoundBase * MySoundEffect, FTransform MyTransform, float MyScaleEmitter)
{
	if (MyParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, MyParticle, MyTransform.GetLocation(), MyTransform.Rotator(), MyTransform.GetScale3D(), false, EPSCPoolMethod::None);
	}

	if (MySoundEffect != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, MySoundEffect, MyTransform.GetLocation());
	}
}
