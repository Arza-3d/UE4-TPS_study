#include "TPS_Projectile.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "TimerManager.h"
#include "TPS_FunctionLibrary.h"

ATPS_Projectile::ATPS_Projectile(/*FProjectile projectile*/)
{
	/*ProjectileMuzzle = projectile.Muzzle;
	ProjectileTrail = projectile.Trail;
	ProjectileHit = projectile.Hit;*/
	//bAllowTickBeforeBeginPlay = false;
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	//MovementComp->bSimulationEnabled = false;

	//static ConstructorHelpers::FClassFinder<UParticleSystemComponent> PrimaryTrail(TEXT("/ParagonWraith/FX/Particles/Abilities/Primary/FX/P_Wraith_Primary_Trail"));
	//check(Curve.Succeeded());
	

	//FloatCurve = Curve.Object;
	ParticleComp->Activate(true);
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
	ParticleComp->SetupAttachment(CollisionComp);
}

void ATPS_Projectile::SetUpProjectile(FProjectile MyProjectile) {
	ProjectileMuzzle = MyProjectile.Muzzle;
	ProjectileTrail = MyProjectile.Trail;
	ProjectileHit = MyProjectile.Hit;
	MovementComp->InitialSpeed = ProjectileMuzzle.InitialSpeedAndOther[0];
	MovementComp->ProjectileGravityScale = (ProjectileMuzzle.InitialSpeedAndOther.Num() > 2) ? ProjectileMuzzle.InitialSpeedAndOther[2] : 0.0f;	
}
void ATPS_Projectile::BeginPlay() {
	Super::BeginPlay();
	PlayFX(ProjectileMuzzle.MuzzleFX.VisualEffect, ProjectileMuzzle.MuzzleFX.SoundEffect, GetActorTransform(), 1.0f);
	ParticleComp->SetTemplate(UTPS_FunctionLibrary::GetRandomParticle(ProjectileTrail.TrailFX.VisualEffect));//SetTemplate(ProjectileTrail.TrailFX.VisualEffect[0]);
	CollisionComp->OnComponentHit.AddDynamic(this, &ATPS_Projectile::OnProjectileHit);
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATPS_Projectile::OnProjectileOverlaped);
}

void ATPS_Projectile::DestroySelf() { GetWorld()->DestroyActor(this); }

void ATPS_Projectile::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) {
	PlayFX(ProjectileHit.HitFX[0].VisualEffect, ProjectileHit.HitFX[0].SoundEffect, GetActorTransform(), 1.0f);
	ParticleComp->DestroyComponent();
	GetWorldTimerManager().ClearTimer(TimerDestroy);
	GetWorldTimerManager().SetTimer(TimerDestroy, this, &ATPS_Projectile::DestroySelf, 3.0f);
}
void ATPS_Projectile::PlayFX(TArray<UParticleSystem*> MyParticles, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter) {
	
	if (MyParticles[0] != nullptr) {
		UParticleSystem* MyParticle = UTPS_FunctionLibrary::GetRandomParticle(MyParticles);
		UGameplayStatics::SpawnEmitterAtLocation(this, MyParticle, MyTransform.GetLocation(), MyTransform.Rotator(), MyTransform.GetScale3D(), false, EPSCPoolMethod::None);
	}
	if (MySoundEffect) {
		UGameplayStatics::PlaySoundAtLocation(this, MySoundEffect, MyTransform.GetLocation());
	}
}
void ATPS_Projectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult & Hit) {
	UE_LOG(LogTemp, Log, TEXT("projectile hit something"));
	GetWorldTimerManager().ClearTimer(TimerDestroy);
	GetWorldTimerManager().SetTimer(TimerDestroy, this, &ATPS_Projectile::DestroySelf, 3.0f);
	//GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f);

}
void ATPS_Projectile::OnProjectileOverlaped(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) {
	UE_LOG(LogTemp, Log, TEXT("projectile OVERLAP something"));
	GetWorldTimerManager().ClearTimer(TimerDestroy);
	GetWorldTimerManager().SetTimer(TimerDestroy, this, &ATPS_Projectile::DestroySelf, 3.0f);

	

}
