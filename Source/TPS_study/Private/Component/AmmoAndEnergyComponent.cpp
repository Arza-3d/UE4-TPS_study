#include "Component/AmmoAndEnergyComponent.h"
#include "Component/RangedWeaponComponent.h"

void UAmmoAndEnergyComponent::AddAmmo(const EAmmoType InAmmoType, const int32 AdditionalAmmo)
{
	switch (InAmmoType)
	{
	case EAmmoType::StandardAmmo:
		AmmunitionCount.StandardAmmo += AdditionalAmmo;
		break;

	case EAmmoType::RifleAmmo:
		AmmunitionCount.RifleAmmo += AdditionalAmmo;
		break;

	case EAmmoType::ShotgunAmmo:
		AmmunitionCount.ShotgunAmmo += AdditionalAmmo;
		break;

	case EAmmoType::Rocket:
		AmmunitionCount.Rocket += AdditionalAmmo;
		break;

	case EAmmoType::Arrow:
		AmmunitionCount.Arrow += AdditionalAmmo;
		break;

	case EAmmoType::Grenade:
		AmmunitionCount.Grenade += AdditionalAmmo;
		break;

	case EAmmoType::Mine:
		AmmunitionCount.Mine += AdditionalAmmo;
		break;

	default:
		break;
	}
}

void UAmmoAndEnergyComponent::AddEnergy(const EEnergyType InEnergyType, const float AdditionalEnergy)
{
	switch (InEnergyType)
	{
	/*case EEnergyType::MP:
		Shooter->CharacterStat.MP += AdditionalEnergy;
		break;*/

	case EEnergyType::Battery:
		EnergyExternal.Battery += AdditionalEnergy;
		break;

	case EEnergyType::Fuel:
		EnergyExternal.Fuel += AdditionalEnergy;
		break;

	default:
		break;
	}
}

bool UAmmoAndEnergyComponent::IsAmmoEnough()
{
	if (RangedWeaponComponent->CurrentWeapon.WeaponCost == EWeaponCost::Nothing)
	{
		return true;
	}

	switch (RangedWeaponComponent->CurrentWeapon.WeaponCost)
	{
	case EWeaponCost::Ammo:
		return IsAmmoEnough(RangedWeaponComponent->CurrentWeapon.AmmoType);

	case EWeaponCost::Energy:
		return IsAmmoEnough(RangedWeaponComponent->CurrentWeapon.EnergyType);

	default:
		return false;
	}
}

bool UAmmoAndEnergyComponent::IsAmmoEnough(const int32 InAmmo)
{
	bool bAmmoIsEmpty = InAmmo <= 0;

	if (bAmmoIsEmpty) OnAmmoOut.Broadcast(this);

	return !bAmmoIsEmpty;
}

bool UAmmoAndEnergyComponent::IsAmmoEnough(const EAmmoType InAmmoType)
{
	switch (InAmmoType)
	{
	case EAmmoType::StandardAmmo:
		return IsAmmoEnough(AmmunitionCount.StandardAmmo);

	case EAmmoType::RifleAmmo:
		return IsAmmoEnough(AmmunitionCount.RifleAmmo);

	case EAmmoType::ShotgunAmmo:
		return IsAmmoEnough(AmmunitionCount.ShotgunAmmo);

	case EAmmoType::Rocket:
		return IsAmmoEnough(AmmunitionCount.Rocket);

	case EAmmoType::Arrow:
		return IsAmmoEnough(AmmunitionCount.Arrow);

	case EAmmoType::Grenade:
		return IsAmmoEnough(AmmunitionCount.Grenade);

	case EAmmoType::Mine:
		return IsAmmoEnough(AmmunitionCount.Mine);

	default:
		return false;
	}
}

bool UAmmoAndEnergyComponent::IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot)
{
	bool bIsNotEnoughEnergy = MyEnergy < MyEnergyPerShot;

	if (bIsNotEnoughEnergy) OnEnergyOut.Broadcast(this, MyEnergy, MyEnergyPerShot);

	return !bIsNotEnoughEnergy;
}

bool UAmmoAndEnergyComponent::IsAmmoEnough(const EEnergyType InEnergyType)
{
	if (RangedWeaponComponent == nullptr) return false;

	switch (InEnergyType)
	{
	case EEnergyType::MP:
		return IsAmmoEnough(EnergyExternal.MP, RangedWeaponComponent->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Fuel:
		return IsAmmoEnough(EnergyExternal.Fuel, RangedWeaponComponent->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Battery:
		return IsAmmoEnough(EnergyExternal.Battery, RangedWeaponComponent->CurrentWeapon.EnergyUsePerShot);

	case EEnergyType::Overheat:
		return IsWeaponNotOverheating();

	default:
		return false;
	}
}

bool UAmmoAndEnergyComponent::IsWeaponNotOverheating()
{
	bool bIsOverheat = EnergyExternal.Overheat >= 100.0f;

	if (bIsOverheat) OnOverhating.Broadcast(this);

	return !bIsOverheat;
}

// Sets default values for this component's properties
UAmmoAndEnergyComponent::UAmmoAndEnergyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAmmoAndEnergyComponent::BeginPlay()
{
	Super::BeginPlay();
	RangedWeaponComponent = GetComponentSibling<URangedWeaponComponent>();
}


FAmmoCount UAmmoAndEnergyComponent::GetAllAmmo() const
{
	return AmmunitionCount;
}
