#include "RangedWeaponComponent.h"
//#include "TPSFunctionLibrary.h"
#include "TPShooterCharacter.h"

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
		Shooter->CurrentWeapon = CurrentWeaponMode.Weapon;
		Shooter->CurrentProjectile = CurrentWeaponMode.Projectile;

		if (Shooter->CurrentWeapon.FireRateAndOther.Num() >= 2)
		{
			Shooter->MaxFireHoldTime = Shooter->CurrentWeapon.FireRateAndOther[1];
		}
		else
		{
			Shooter->MaxFireHoldTime = Shooter->CurrentWeapon.FireRateAndOther[0];
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
	if (Shooter->CurrentWeapon.WeaponCost == EWeaponCost::Nothing)
	{
		return true;
	}

	switch (Shooter->CurrentWeapon.WeaponCost)
	{
	case EWeaponCost::Ammo:
		return IsAmmoEnough(Shooter->CurrentWeapon.AmmoType);

	case EWeaponCost::Energy:
		return IsAmmoEnough(Shooter->CurrentWeapon.EnergyType);

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
		return IsAmmoEnough(Shooter->Ammunition.StandardAmmo);

	case EAmmoType::RifleAmmo:
		return IsAmmoEnough(Shooter->Ammunition.RifleAmmo);

	case EAmmoType::ShotgunAmmo:
		return IsAmmoEnough(Shooter->Ammunition.ShotgunAmmo);

	case EAmmoType::Rocket:
		return IsAmmoEnough(Shooter->Ammunition.Rocket);

	case EAmmoType::Arrow:
		return IsAmmoEnough(Shooter->Ammunition.Arrow);

	case EAmmoType::Grenade:
		return IsAmmoEnough(Shooter->Ammunition.Grenade);

	case EAmmoType::Mine:
		return IsAmmoEnough(Shooter->Ammunition.Mine);

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
		return IsAmmoEnough(Shooter->CharacterStat.MP, Shooter->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Fuel:
		return IsAmmoEnough(Shooter->EnergyExternal.Fuel, Shooter->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Battery:
		return IsAmmoEnough(Shooter->EnergyExternal.Battery, Shooter->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Overheat:
		return IsWeaponNotOverheating();

	default:
		return false;
	}
}

bool URangedWeaponComponent::IsWeaponNotOverheating()
{
	bool bIsOverheat = Shooter->EnergyExternal.Overheat >= 100.0f;

	if (bIsOverheat)
	{
		Shooter->OnWeaponOverheats();
	}

	return !bIsOverheat;
}