#include "RangedWeaponComponent.h"
#include "TPS_Projectile.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "TPShooterCharacter.h"
#include "Blueprint/UserWidget.h"


URangedWeaponComponent::URangedWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void URangedWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	Shooter = Cast<ATPShooterCharacter>(GetOwner());

	if (WeaponTable != nullptr)
	{
		WeaponNames = WeaponTable->GetRowNames();
		SetWeaponMode(0);
	}

	SetWeaponMesh();
}

FName URangedWeaponComponent::GetWeaponName() const
{
	return WeaponNames[WeaponIndex];
}

//===========================================================================
// private function:
//===========================================================================

int32 URangedWeaponComponent::GetWeaponIndex() const
{
	return WeaponIndex;
}

int32 URangedWeaponComponent::GetLastWeaponIndex() const
{
	return LastWeaponIndex;
}

void URangedWeaponComponent::SetWeaponIndex(bool isUp)
{
	LastWeaponIndex = WeaponIndex;

	if (Shooter->IsAbleToSwitchWeapon())
	{
		int32 counter = isUp ? 1 : -1;
		int32 withinRange = (WeaponIndex + counter) % WeaponNames.Num();

		WeaponIndex = (withinRange >= 0) ? withinRange : WeaponNames.Num() - 1;
		SetWeaponMode(WeaponIndex); // TEST2
		Shooter->OnSwitchWeapon();
	}
}

void URangedWeaponComponent::SetWeaponIndex(const int32 InNumber)
{

	if (InNumber >= WeaponNames.Num()) { return; }

	LastWeaponIndex = WeaponIndex;

	if ((WeaponNames.Num() > WeaponIndex) && Shooter->IsAbleToSwitchWeapon())
	{
		WeaponIndex = InNumber;
		SetWeaponMode(WeaponIndex); // TEST
		Shooter->OnSwitchWeapon();
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

		Shooter->ShooterState = CurrentWeaponMode.Shooter;
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

void URangedWeaponComponent::SetWeaponMesh()
{
	USkeletalMeshComponent* weaponMesh = Shooter->GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(weaponMesh);
}

bool URangedWeaponComponent::IsAmmoEnough()
{
	if (CurrentWeapon.WeaponCost == EWeaponCost::Nothing)
	{
		return true;
	}

	switch (CurrentWeapon.WeaponCost)
	{
	case EWeaponCost::Ammo:
		return IsAmmoEnough(CurrentWeapon.AmmoType);

	case EWeaponCost::Energy:
		return IsAmmoEnough(CurrentWeapon.EnergyType);

	default:
		return false;
	}
}

bool URangedWeaponComponent::IsAmmoEnough(const int32 InAmmo)
{
	bool bAmmoIsEmpty = InAmmo <= 0;

	if (bAmmoIsEmpty)
	{
		Shooter->OnNoAmmo();
	}

	return !bAmmoIsEmpty;
}

bool URangedWeaponComponent::IsAmmoEnough(const EAmmoType InAmmoType)
{
	switch (InAmmoType)
	{
	case EAmmoType::StandardAmmo:
		return IsAmmoEnough(Ammunition.StandardAmmo);

	case EAmmoType::RifleAmmo:
		return IsAmmoEnough(Ammunition.RifleAmmo);

	case EAmmoType::ShotgunAmmo:
		return IsAmmoEnough(Ammunition.ShotgunAmmo);

	case EAmmoType::Rocket:
		return IsAmmoEnough(Ammunition.Rocket);

	case EAmmoType::Arrow:
		return IsAmmoEnough(Ammunition.Arrow);

	case EAmmoType::Grenade:
		return IsAmmoEnough(Ammunition.Grenade);

	case EAmmoType::Mine:
		return IsAmmoEnough(Ammunition.Mine);

	default:
		return false;
	}
}

bool URangedWeaponComponent::IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot)
{
	bool bIsNotEnoughEnergy = MyEnergy < MyEnergyPerShot;

	if (bIsNotEnoughEnergy)
	{
		Shooter->OnNoEnergy();
	}

	return !bIsNotEnoughEnergy;
}

bool URangedWeaponComponent::IsAmmoEnough(const EEnergyType InEnergyType)
{
	switch (InEnergyType)
	{
	case EEnergyType::MP:
		return IsAmmoEnough(Shooter->CharacterStat.MP, CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Fuel:
		return IsAmmoEnough(EnergyExternal.Fuel, CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Battery:
		return IsAmmoEnough(EnergyExternal.Battery, CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Overheat:
		return IsWeaponNotOverheating();

	default:
		return false;
	}
}

bool URangedWeaponComponent::IsWeaponNotOverheating()
{
	bool bIsOverheat = EnergyExternal.Overheat >= 100.0f;

	if (bIsOverheat)
	{
		Shooter->OnWeaponOverheats();
	}

	return !bIsOverheat;
}

void URangedWeaponComponent::FireAutomaticTrigger()
{
	if (!(bIsTriggerPressed && IsWeaponAbleToFire()))
	{
		return;
	}

	FireStandardTrigger();
}

bool URangedWeaponComponent::IsWeaponAbleToFire()
{
	return Shooter->GetIsAiming() && bIsFireRatePassed && IsAmmoEnough();
}

void URangedWeaponComponent::FireAutomaticTriggerOnePress()
{
	if (bOnePressToggle && IsWeaponAbleToFire())
	{
		FireStandardTrigger();
	}
}

void URangedWeaponComponent::FireHold()
{

	Shooter->GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	Shooter->GetWorldTimerManager().SetTimer(TimerOfHoldTrigger, this, &URangedWeaponComponent::CountHoldTriggerTime, HoldTimeRateCount, true);
}

void URangedWeaponComponent::CountHoldTriggerTime()
{
	HoldTime += HoldTimeRateCount;

	if (!bMaxHoldIsReach)
	{
		if (HoldTime >= MaxFireHoldTime)
		{
			bMaxHoldIsReach = true;
			Shooter->OnMaxFireHold();
		}
	}
}

void URangedWeaponComponent::FireReleaseAfterHold()
{
	Shooter->GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	if (bMaxHoldIsReach)
	{
		FireStandardTrigger();
		Shooter->OnMaxFireHoldRelease();
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
	PlayFireMontage();
	Shooter->OnWeaponFires();

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
	switch (AmmoType)
	{
	case EAmmoType::StandardAmmo:
		FireProjectile(&Ammunition.StandardAmmo);
		break;

	case EAmmoType::RifleAmmo:
		FireProjectile(&Ammunition.RifleAmmo);
		break;

	case EAmmoType::ShotgunAmmo:
		FireProjectile(&Ammunition.ShotgunAmmo);
		break;

	case EAmmoType::Rocket:
		FireProjectile(&Ammunition.Rocket);
		break;

	case EAmmoType::Arrow:
		FireProjectile(&Ammunition.Arrow);
		break;

	case EAmmoType::Grenade:
		FireProjectile(&Ammunition.Grenade);
		break;

	case EAmmoType::Mine:
		FireProjectile(&Ammunition.Mine);
		break;

	default:
		break;
	}
}

void URangedWeaponComponent::FireProjectile(const EEnergyType EnergyType)
{
	switch (EnergyType)
	{
	case EEnergyType::MP:
		FireProjectile(&Shooter->CharacterStat.MP);
		break;

	case EEnergyType::Battery:
		FireProjectile(&EnergyExternal.Battery);
		break;

	case EEnergyType::Fuel:
		FireProjectile(&EnergyExternal.Fuel);
		break;

	case EEnergyType::Overheat:
		FireProjectile(&EnergyExternal.Overheat);
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
			Shooter->OnNoMoreAmmoDuringFire();
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
				Shooter->OnNoMoreAmmoDuringFire();
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
				Shooter->OnNoMoreAmmoDuringFire();
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

void URangedWeaponComponent::PlayFireMontage()
{
	UAnimMontage* fireMontage;

	if (Shooter->ShooterState.CharacterWeaponMontage.Num() > 0)
	{
		fireMontage = Shooter->ShooterState.CharacterWeaponMontage[0];

		if (fireMontage)
		{
			float playRate = UTPSFunctionLibrary::GetNewPlayRateForMontage(CurrentWeapon.FireRateAndOther[0], fireMontage);
			Shooter->PlayAnimMontage(fireMontage, playRate);
		}
	}
}

void URangedWeaponComponent::TimerFireRateStart()
{
	bIsFireRatePassed = false;

	Shooter->GetWorldTimerManager().ClearTimer(FireRateTimer);
	Shooter->GetWorldTimerManager().SetTimer(FireRateTimer, this, &URangedWeaponComponent::TimerFireRateReset, CurrentWeapon.FireRateAndOther[0]);
}

void URangedWeaponComponent::TimerFireRateReset()
{
	bIsFireRatePassed = true;
	Shooter->GetWorldTimerManager().ClearTimer(FireRateTimer);

	if (CurrentWeapon.Trigger == ETriggerMechanism::AutomaticTrigger)
	{
		FireAutomaticTrigger();
	}
	else if (CurrentWeapon.Trigger == ETriggerMechanism::OnePressAutoTrigger)
	{
		FireAutomaticTriggerOnePress();
	}
}

void URangedWeaponComponent::FlipOnePressTriggerSwitch()
{
	bOnePressToggle = (bOnePressToggle) ? false : true;
}

FRotator URangedWeaponComponent::GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(Shooter);

	FHitResult HitTrace;
	FVector StartTrace = Shooter->GetFollowCamera()->GetComponentLocation();
	FRotator CameraRotation = Shooter->GetFollowCamera()->GetComponentRotation();
	FVector LookDirection = UKismetMathLibrary::GetForwardVector(CameraRotation);
	FVector EndTrace = StartTrace + LookDirection * 100.0f * 3000.0f;
	FVector TargetLocation;
	FRotator MuzzleLookRotation;

	if (Shooter->GetWorld()->LineTraceSingleByChannel(HitTrace, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{
		TargetLocation = HitTrace.Location;
		MuzzleLookRotation = UKismetMathLibrary::FindLookAtRotation(SocketTransform.GetLocation(), TargetLocation);
	}

	return MuzzleLookRotation;
}

ETriggerMechanism URangedWeaponComponent::GetTriggerMechanism() const
{
	return CurrentWeapon.Trigger;
}

bool URangedWeaponComponent::GetIsTriggerPressed() const
{
	return bIsTriggerPressed;
}

void URangedWeaponComponent::FirePress()
{
	bIsTriggerPressed = true;
	FlipOnePressTriggerSwitch();

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
	{
		FireReleaseAfterHold();
	}
}

void URangedWeaponComponent::AddAmmo(const int32 addAmmo, const EAmmoType ammoType)
{
	switch (ammoType)
	{
	case EAmmoType::StandardAmmo:
		Ammunition.StandardAmmo += addAmmo;
		break;

	case EAmmoType::RifleAmmo:
		Ammunition.RifleAmmo += addAmmo;
		break;

	case EAmmoType::ShotgunAmmo:
		Ammunition.ShotgunAmmo += addAmmo;
		break;

	case EAmmoType::Rocket:
		Ammunition.Rocket += addAmmo;
		break;

	case EAmmoType::Arrow:
		Ammunition.Arrow += addAmmo;
		break;

	case EAmmoType::Grenade:
		Ammunition.Grenade += addAmmo;
		break;

	case EAmmoType::Mine:
		Ammunition.Mine += addAmmo;
	}
}