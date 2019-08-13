#include "TPS_Weapon.h"
#include "TPS_studyCharacter.h"

UTPS_Weapon::UTPS_Weapon()
{
	AActor* myCharacter = GetOwner();
	TPSCharacter = Cast<ATPS_studyCharacter>(myCharacter);
}

void UTPS_Weapon::MainFire()
{


}

void UTPS_Weapon::GetCurrentWeaponMode(int weaponIndex)
{
	FName currentWeaponName = WeaponNames[weaponIndex];
	static const FString contextString(TEXT("Weapon Mode"));
	struct FWeaponModeCompact* weaponModeRow;

	weaponModeRow = RangedWeaponTable->FindRow<FWeaponModeCompact>(currentWeaponName, contextString, true);

	if (weaponModeRow)
	{
		FWeaponMode currentWeaponMode = weaponModeRow->WeaponMode;
		TPSCharacter->ShooterState = currentWeaponMode.Shooter;
		CurrentWeapon = currentWeaponMode.Weapon;
		CurrentProjectile = currentWeaponMode.Projectile;
	}
}
