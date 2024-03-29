#include "TPS_Projectile.h"
#include "CustomCollisionChannel.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "ConstructorHelpers.h"
#include "Engine/Engine.h"// delete later
#include "Kismet/GameplayStatics.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "ProjectileParticleDataAsset.h"
#include "ProjectileSoundDataAsset.h"
#include "TimerManager.h" // delete later
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
	CollisionComp->SetCollisionObjectType(ECC_PlayerProjectile);//ProjectileMuzzle.CollisionComp);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Enemy, ECR_Block);
	//CollisionComp->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);

	/*if (CollisionComp->GetCollisionObjectType() == ECC_PlayerProjectile)//ECC_GameTraceChannel1)
	{
		CollisionComp->SetCollisionResponseToChannel(ECC_Player, ECR_Ignore);
		CollisionComp->SetCollisionResponseToChannel(ECC_Enemy, ECR_Block);
		UE_LOG(LogTemp, Log, TEXT("it's player projectile"));

	}
	else if (CollisionComp->GetCollisionObjectType() == ECC_EnemyProjectile)
	{
		CollisionComp->SetCollisionResponseToChannel(ECC_Player, ECR_Block);
		CollisionComp->SetCollisionResponseToChannel(ECC_Enemy, ECR_Ignore);
		UE_LOG(LogTemp, Log, TEXT("it's enemy projectile"));
	}*/

	RootComponent = CollisionComp;
	ProjectileTrailParticle->SetupAttachment(CollisionComp);

	UE_LOG(LogTemp, Log, TEXT("Event construct!"));
}

void ATPS_Projectile::SetUpProjectile(FProjectile MyProjectile, APawn* InInstigator) 
{
	ProjectileParticleObject = MyProjectile.ProjectileParticle;
	ProjectileSoundObject = MyProjectile.ProjectileSound;
	ProjectileData = MyProjectile.ProjectileData;
	
	if (ProjectileData.SpeedxGravityxScale.Num() > 0) 
	{
		MovementComp->InitialSpeed = ProjectileData.SpeedxGravityxScale[0];
	}
	else
	{
		FProjectileData defaultProjectile;

		MovementComp->InitialSpeed = defaultProjectile.SpeedxGravityxScale[0];
	}

	MovementComp->ProjectileGravityScale = (ProjectileData.SpeedxGravityxScale.Num() > 2) ? ProjectileData.SpeedxGravityxScale[2] : 0.0f;

	UE_LOG(LogTemp, Log, TEXT("Event setup!"));
}

void ATPS_Projectile::BeginPlay() 
{
	Super::BeginPlay();

	if (ProjectileParticleObject) 
	{
		float particleScale = (ProjectileData.SpeedxGravityxScale.Num() >= 3) ? ProjectileData.SpeedxGravityxScale[2] : 1.0f;

		SetActorScale3D(FVector(particleScale));

		TArray<UParticleSystem*> muzzleParticle = ProjectileParticleObject->ProjectileParticle.MuzzleParticle;

		if (muzzleParticle.Num() >= 0 && muzzleParticle[0] != nullptr)
			UGameplayStatics::SpawnEmitterAtLocation(
				this, 
				muzzleParticle[0],
				GetActorLocation(), 
				GetActorRotation(), 
				GetActorScale(), 
				true, 
				EPSCPoolMethod::None
			);

		TArray<UParticleSystem*> trailParticle = ProjectileParticleObject->ProjectileParticle.TrailParticle;

		if (trailParticle.Num() > 0 && trailParticle[0] != nullptr)
			ProjectileTrailParticle->SetTemplate(
				trailParticle[0]
			);
	}

	if (ProjectileSoundObject) {
		USoundBase* muzzleSound = ProjectileSoundObject->ProjectileSound.MuzzleSound;

		if (muzzleSound)
			UGameplayStatics::PlaySoundAtLocation(
				this,
				muzzleSound,
				GetActorLocation()
			);
	}

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ATPS_Projectile::ShowOverlapObjectData);
	CollisionComp->OnComponentHit.AddDynamic(this, &ATPS_Projectile::ShowHitObjectData);

	UE_LOG(LogTemp, Log, TEXT("Event BEGINPLAY!"));
	UKismetSystemLibrary::PrintString(this, FString("Projectile Begin Play"), true, false, FLinearColor::Red, 5.0f);
}

void ATPS_Projectile::DestroySelf() 
{ 
	GetWorld()->DestroyActor(this); 

	UE_LOG(LogTemp, Log, TEXT("Event DESTROY!"));
}

void ATPS_Projectile::NotifyHit(UPrimitiveComponent * MyComp, AActor * Other, UPrimitiveComponent * OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult & Hit) 
{
	TArray<UParticleSystem*> hitParticle = ProjectileParticleObject->ProjectileParticle.HitParticle;

	if (ProjectileParticleObject)
	{
		if (hitParticle.Num() >= 0 && hitParticle[0] != nullptr)
			UGameplayStatics::SpawnEmitterAtLocation(
				this,
				hitParticle[0],
				GetActorLocation(),
				GetActorRotation(),
				GetActorScale(),
				true,
				EPSCPoolMethod::None
			);

		if (ProjectileTrailParticle != nullptr)
			ProjectileTrailParticle->DestroyComponent();
	}

	if (ProjectileSoundObject)
	{
		TArray<USoundBase*> hitSound = ProjectileSoundObject->ProjectileSound.HitAndTrailSound;

		if (hitSound.Num() > 0 && hitSound[0] != nullptr)
			UGameplayStatics::PlaySoundAtLocation(
				this,
				hitSound[0],
				GetActorLocation()
			);
	}

	GetWorldTimerManager().ClearTimer(TimerDestroy);
	GetWorldTimerManager().SetTimer(TimerDestroy, this, &ATPS_Projectile::DestroySelf, 3.0f);
}

void ATPS_Projectile::ShowOverlapObjectData(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	GEngine->ClearOnScreenDebugMessages();
	FString sumStr;
	FString tempStr = UKismetSystemLibrary::GetObjectName(OverlappedComponent);
	sumStr = tempStr.Append(FString(" has object type of "));
	tempStr = UEnum::GetValueAsString(OverlappedComponent->GetCollisionObjectType());
	sumStr = sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Green, 5.0f);
	sumStr = FString(" OVERLAP!!! with ");
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::White, 5.0f);
	sumStr = UKismetSystemLibrary::GetObjectName(OtherActor);
	//sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Blue, 5.0f);
	sumStr = UKismetSystemLibrary::GetObjectName(OtherComp);;
	sumStr.Append(" that has object type of ");
	tempStr = UEnum::GetValueAsString(OtherComp->GetCollisionObjectType());
	sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Red, 5.0f);
	USkeletalMeshComponent* skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	if (skeleton)
	{
		
		//sumStr = skeleton->Bonehit
	}
}

void ATPS_Projectile::ShowHitObjectData(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GEngine->ClearOnScreenDebugMessages();
	FString sumStr;
	FString tempStr = UKismetSystemLibrary::GetObjectName(HitComponent);
	sumStr = tempStr.Append(FString(" has object type of "));
	tempStr = UEnum::GetValueAsString(HitComponent->GetCollisionObjectType());
	sumStr = sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Green, 5.0f);
	sumStr = FString(" HIT!!! with ");
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::White, 5.0f);
	sumStr = UKismetSystemLibrary::GetObjectName(OtherActor);
	//sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Blue, 5.0f);
	sumStr = UKismetSystemLibrary::GetObjectName(OtherComp);;
	sumStr.Append(" that has object type of ");
	tempStr = UEnum::GetValueAsString(OtherComp->GetCollisionObjectType());
	sumStr.Append(tempStr);
	UKismetSystemLibrary::PrintString(this, sumStr, true, false, FLinearColor::Red, 5.0f);
	USkeletalMeshComponent* skeleton = Cast<USkeletalMeshComponent>(OtherComp);
	if (skeleton)
	{

		//sumStr = skeleton->Bonehit
	}

}

/*void ATPS_Projectile::SpawnFX(TArray<UParticleSystem*> MyParticles, USoundBase* MySoundEffect, FTransform MyTransform, float MyScaleEmitter) 
{
	if (MyParticles.Num() > 0) 
	{
		UParticleSystem* MyParticle = UTPSFunctionLibrary::GetRandomParticle(MyParticles);
		UGameplayStatics::SpawnEmitterAtLocation(this, MyParticle, MyTransform.GetLocation(), MyTransform.Rotator(), MyTransform.GetScale3D(), false, EPSCPoolMethod::None);
	}
	
	if (MySoundEffect) 
	UGameplayStatics::PlaySoundAtLocation(this, MySoundEffect, MyTransform.GetLocation());
}*/

/*void ATPS_Projectile::SpawnFX(UParticleSystem* MyParticle, USoundBase * MySoundEffect, FTransform MyTransform, float MyScaleEmitter)
{
	if (MyParticle)
	UGameplayStatics::SpawnEmitterAtLocation(this, MyParticle, MyTransform.GetLocation(), MyTransform.Rotator(), MyTransform.GetScale3D(), false, EPSCPoolMethod::None);
	
	if (MySoundEffect)
	UGameplayStatics::PlaySoundAtLocation(this, MySoundEffect, MyTransform.GetLocation());
	
}*/
