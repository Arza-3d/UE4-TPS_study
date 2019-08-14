#include "TPS_Projectile.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TPS_FunctionLibrary.h"

ATPS_Projectile::ATPS_Projectile(/*FProjectile projectile*/)
{
	/*ProjectileMuzzle = projectile.Muzzle;
	ProjectileTrail = projectile.Trail;
	ProjectileHit = projectile.Hit;*/

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->AlwaysLoadOnClient = true;
	CollisionComp->AlwaysLoadOnServer = true;
	CollisionComp->bTraceComplexOnMove = true;
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//CollisionComp->SetCollisionObjectType(ECollisionChannel::);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	RootComponent = CollisionComp;
}

/*void ATPS_Projectile::SetProjectileMuzzle()
{

	//UGameplayStatics::SpawnEmitterAtLocation(this);
}*/

void ATPS_Projectile::SetUpProjectile(FProjectile myProjectile)
{
	ProjectileMuzzle = myProjectile.Muzzle;
	ProjectileTrail = myProjectile.Trail;
	ProjectileHit = myProjectile.Hit;
}
