#include "Component/RangedWeaponComponent.h"

#include "Camera/CameraComponent.h"
#include "Gameframework/Character.h"
#include "TimerManager.h"
//#include "UObject/ConstructorHelpers.h"

#include "Actor/TPS_Projectile.h"
#include "Component/AimingComponent.h"
#include "Component/AmmoAndEnergyComponent.h"
#include "Component/HPandMPComponent.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

//===========================================================================
// public function:
//===========================================================================

URangedWeaponComponent::URangedWeaponComponent()
{
	if (!WeaponTable) SetUpVariables(bShouldDoCheckFile);
}

//=================
// Getter (public):
//=================


int32 URangedWeaponComponent::GetWeaponIndex() const { return WeaponIndex; }

int32 URangedWeaponComponent::GetLastWeaponIndex() const { return LastWeaponIndex; }

FName URangedWeaponComponent::GetWeaponName() const { return WeaponNames[WeaponIndex]; }

bool URangedWeaponComponent::GetIsTriggerPressed() const { return bIsTriggerPressed; }

ETriggerMechanism URangedWeaponComponent::GetTriggerMechanism() const { return CurrentWeapon.Trigger; }

//==================================
// Function for Controller (public):
//==================================

void URangedWeaponComponent::FirePress()
{
	bIsTriggerPressed = true;
	bOnePressToggle = (bOnePressToggle) ? false : true;

	if (!IsWeaponAbleToFire()) { return; }

	switch (CurrentWeapon.Trigger)
	{
	case ETriggerMechanism::PressTrigger:
		FireStandardTrigger();
		break;

	case ETriggerMechanism::AutomaticTrigger:
		FireAutomaticTrigger();
		break;

	case ETriggerMechanism::ReleaseTrigger:
		FireHold();
		break;

	case ETriggerMechanism::OnePressAutoTrigger:
		FireAutomaticTriggerOnePress();
		break;

	default:
		FireStandardTrigger();
	}
}

void URangedWeaponComponent::FireRelease()
{
	bIsTriggerPressed = false;

	if (CurrentWeapon.Trigger == ETriggerMechanism::ReleaseTrigger)
	FireReleaseAfterHold();
}

void URangedWeaponComponent::SetWeaponIndexWithNumpad(const int32 InNumber) { SetWeaponIndex(InNumber); }

void URangedWeaponComponent::SetWeaponIndexWithMouseWheel(const bool bIsUp) { SetWeaponIndex(bIsUp); }

//===========================================================================
// protected function:
//===========================================================================

void URangedWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent = GetComponentSibling<UCameraComponent>();
	AimingComponent = GetComponentSibling<UAimingComponent>();
	AmmoComponent = GetComponentSibling<UAmmoAndEnergyComponent>();
	MPComponent = GetComponentSibling<UHPandMPComponent>();

	if (!WeaponTable) SetUpVariables(bShouldDoCheckFile);

	WeaponNames = WeaponTable->GetRowNames();
	SetWeaponMode(0);
	SetWeaponMesh();
}

void URangedWeaponComponent::SetUpVariables(bool bShouldCheck)
{
	WeaponTable = GetThisFile<UDataTable>(TEXT("DataTable'/Game/Character/Table/WeaponTable.WeaponTable'"));
}

//===========================================================================
// private function:
//===========================================================================

void URangedWeaponComponent::SetWeaponIndex(bool isUp)
{
	LastWeaponIndex = WeaponIndex;

	if (true/*Shooter->IsAbleToSwitchWeapon()*/)
	{
		int32 inCounter = isUp ? 1 : -1;
		int32 withinRange = (WeaponIndex + inCounter) % WeaponNames.Num();

		WeaponIndex = (withinRange >= 0) ? withinRange : WeaponNames.Num() - 1;
		SetWeaponMode(WeaponIndex);
		OnSwitchWeapon.Broadcast(this);
	}
}

void URangedWeaponComponent::SetWeaponIndex(const int32 InNumber)
{
	if (InNumber >= WeaponNames.Num()) { return; }

	LastWeaponIndex = WeaponIndex;

	if ((WeaponNames.Num() > WeaponIndex)/* && Shooter->IsAbleToSwitchWeapon()*/)
	{
		WeaponIndex = InNumber;
		SetWeaponMode(WeaponIndex);
		OnSwitchWeapon.Broadcast(this);
	}
}

void URangedWeaponComponent::SetWeaponMode(const int32 MyWeaponIndex)
{
	FName CurrentWeaponName = WeaponNames[MyWeaponIndex];
	static const FString ContextString(TEXT("Weapon Mode"));
	struct FWeaponModeCompact* WeaponModeRow;
	WeaponModeRow = WeaponTable->FindRow<FWeaponModeCompact>(CurrentWeaponName, ContextString, true);

	if (WeaponModeRow)
	{
		FWeaponMode CurrentWeaponMode = WeaponModeRow->WeaponMode;

		//Shooter->ShooterState = CurrentWeaponMode.Shooter;
		CurrentWeapon = CurrentWeaponMode.Weapon;
		CurrentProjectile = CurrentWeaponMode.Projectile;

		if (CurrentWeapon.FireRateAndOther.Num() >= 2)
		{
			MaxFireHoldTime = CurrentWeapon.FireRateAndOther[1];
		}
		else
		{
			MaxFireHoldTime = CurrentWeapon.FireRateAndOther[0];
		}
	}
}

//================
// Fire (private):
//================

void URangedWeaponComponent::SetWeaponMesh()
{
	USkeletalMeshComponent* weaponMesh = Cast<ACharacter>(GetOwner())->GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(weaponMesh);
}

void URangedWeaponComponent::FireAutomaticTrigger()
{
	if (bIsTriggerPressed && IsWeaponAbleToFire())
	FireStandardTrigger();
}

bool URangedWeaponComponent::IsWeaponAbleToFire()
{
	if (AimingComponent && AmmoComponent) return AimingComponent->GetIsAiming() && bIsFireRatePassed && AmmoComponent->IsAmmoEnough();
	return bIsFireRatePassed;
}

void URangedWeaponComponent::FireAutomaticTriggerOnePress()
{
	if (bOnePressToggle && IsWeaponAbleToFire())
	FireStandardTrigger();
}

void URangedWeaponComponent::FireHold()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	GetOwner()->GetWorldTimerManager().SetTimer(TimerOfHoldTrigger, this, &URangedWeaponComponent::CountHoldTriggerTime, HoldTimeRateCount, true);
}

void URangedWeaponComponent::CountHoldTriggerTime()
{
	HoldTime += HoldTimeRateCount;

	if (!bMaxHoldIsReach)
	{
		if (HoldTime >= MaxFireHoldTime)
		{
			bMaxHoldIsReach = true;
			OnJustReachMaxHoldTrigger.Broadcast(this, HoldTime, MaxFireHoldTime);
		}
	}
}

void URangedWeaponComponent::FireReleaseAfterHold()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	if (bMaxHoldIsReach)
	{
		FireStandardTrigger();
		OnMaxFireHoldRelease.Broadcast(this, HoldTime, MaxFireHoldTime);
	}
	else if (HoldTime >= CurrentWeapon.FireRateAndOther[0])
	{
		FireStandardTrigger();
	}

	bMaxHoldIsReach = false;
	HoldTime = 0.0f;
}

void URangedWeaponComponent::FireStandardTrigger()
{
	TimerFireRateStart();

	OnFire.Broadcast(this);

	switch (CurrentWeapon.WeaponCost)
	{
	case EWeaponCost::Nothing:
		FireProjectile();
		break;

	case EWeaponCost::Ammo:
		FireProjectile(CurrentWeapon.AmmoType);
		break;

	case EWeaponCost::Energy:
		FireProjectile(CurrentWeapon.EnergyType);
		break;

	default:
		break;
	}
}

void URangedWeaponComponent::FireProjectile(const EAmmoType AmmoType)
{
	if (AmmoComponent)
	{
		switch (AmmoType)
		{
		case EAmmoType::StandardAmmo:
			FireProjectile(&AmmoComponent->AmmunitionCount.StandardAmmo);
			break;

		case EAmmoType::RifleAmmo:
			FireProjectile(&AmmoComponent->AmmunitionCount.RifleAmmo);
			break;

		case EAmmoType::ShotgunAmmo:
			FireProjectile(&AmmoComponent->AmmunitionCount.ShotgunAmmo);
			break;

		case EAmmoType::Rocket:
			FireProjectile(&AmmoComponent->AmmunitionCount.Rocket);
			break;

		case EAmmoType::Arrow:
			FireProjectile(&AmmoComponent->AmmunitionCount.Arrow);
			break;

		case EAmmoType::Grenade:
			FireProjectile(&AmmoComponent->AmmunitionCount.Grenade);
			break;

		case EAmmoType::Mine:
			FireProjectile(&AmmoComponent->AmmunitionCount.Mine);
			break;

		default:
			break;
		}
	}
	else FireProjectile();
}

void URangedWeaponComponent::FireProjectile(const EEnergyType EnergyType)
{
	switch (EnergyType)
	{
	case EEnergyType::MP:
		if (MPComponent){
			FireProjectile(&MPComponent->HealthAndMana.Mana.Current);
		} else {
			FireProjectile();
		} break;

	case EEnergyType::Battery:
		if (AmmoComponent) {
			FireProjectile(&AmmoComponent->EnergyExternal.Battery);
		} else {
			FireProjectile();
		} break;

	case EEnergyType::Fuel:
		if (AmmoComponent) {
			FireProjectile(&AmmoComponent->EnergyExternal.Fuel);
		} else {
			FireProjectile();
		} break;

	case EEnergyType::Overheat:
		if (AmmoComponent) {
			FireProjectile(&AmmoComponent->EnergyExternal.Overheat);
		} else {
			FireProjectile();
		}

	default:
		break;
	}
}

void URangedWeaponComponent::FireProjectile()
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int MuzzleCount = MuzzleName.Num();

	for (int i = 0; i < MuzzleCount; i++)
	{
		SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
	}
}

void URangedWeaponComponent::FireProjectile(int* Ammo)
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int32 MuzzleCount = MuzzleName.Num();
	int32 CurrentAmmo = *Ammo;

	for (int i = 0; i < MuzzleCount; i++)
	{
		if (CurrentAmmo <= 0) {
			AmmoComponent->OnNoMoreAmmoDuringMultipleShot.Broadcast(i);
			break;
		}

		CurrentAmmo--;
		SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
	}
	*Ammo = CurrentAmmo;
}

void URangedWeaponComponent::FireProjectile(float* MyEnergy)
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int32 MuzzleCount = MuzzleName.Num();
	float CurrentEnergy = *MyEnergy;
	float EnergyCostPerShot = CurrentWeapon.EnergyUsePerShot;

	if (CurrentWeapon.EnergyType != EEnergyType::Overheat)
	{
		for (int i = 0; i < MuzzleCount; i++)
		{
			if (CurrentEnergy < EnergyCostPerShot)
			{
				AmmoComponent->OnNoMoreAmmoDuringMultipleShot.Broadcast(i);
				break;
			}
			CurrentEnergy -= EnergyCostPerShot;
			SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
		}
	}
	else
	{
		for (int i = 0; i < MuzzleCount; i++)
		{
			if (CurrentEnergy >= 100.0f)
			{
				AmmoComponent->OnNoMoreAmmoDuringMultipleShot.Broadcast(i);
				break;
			}

			CurrentEnergy += EnergyCostPerShot;
			SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
		}
	}
	*MyEnergy = CurrentEnergy;
}

void URangedWeaponComponent::SpawnProjectile(USceneComponent* MyWeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int32 i)
{
	FTransform MuzzleTransform = MyWeaponInWorld->GetSocketTransform(MuzzleName[i]);
	FTransform SpawnTransform = FTransform(GetNewMuzzleRotationFromLineTrace(MuzzleTransform), MuzzleTransform.GetLocation(), MuzzleTransform.GetScale3D());

	ATPS_Projectile* MyProjectile = MyWorld->SpawnActorDeferred<ATPS_Projectile>(ATPS_Projectile::StaticClass(), SpawnTransform);

	MyProjectile->SetUpProjectile(CurrentProjectile);
	MyProjectile->FinishSpawning(SpawnTransform);
}

void URangedWeaponComponent::TimerFireRateStart()
{
	bIsFireRatePassed = false;

	GetOwner()->GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetOwner()->GetWorldTimerManager().SetTimer(FireRateTimer, this, &URangedWeaponComponent::TimerFireRateReset, CurrentWeapon.FireRateAndOther[0]);
}

void URangedWeaponComponent::TimerFireRateReset()
{
	bIsFireRatePassed = true;
	GetOwner()->GetWorldTimerManager().ClearTimer(FireRateTimer);

	if (CurrentWeapon.Trigger == ETriggerMechanism::AutomaticTrigger)
	{
		FireAutomaticTrigger();
	}
	else if (CurrentWeapon.Trigger == ETriggerMechanism::OnePressAutoTrigger)
	{
		FireAutomaticTriggerOnePress();
	}
}

FRotator URangedWeaponComponent::GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());

	FHitResult HitTrace;
	FVector StartTrace = CameraComponent->GetComponentLocation();
	FRotator CameraRotation = CameraComponent->GetComponentRotation();
	FVector LookDirection = UKismetMathLibrary::GetForwardVector(CameraRotation);
	FVector EndTrace = StartTrace + LookDirection * 100.0f * 3000.0f;
	FVector TargetLocation;
	FRotator MuzzleLookRotation;

	if (GetOwner()->GetWorld()->LineTraceSingleByChannel(HitTrace, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{
		TargetLocation = HitTrace.Location;
		MuzzleLookRotation = UKismetMathLibrary::FindLookAtRotation(SocketTransform.GetLocation(), TargetLocation);
	}
	return MuzzleLookRotation;
}
