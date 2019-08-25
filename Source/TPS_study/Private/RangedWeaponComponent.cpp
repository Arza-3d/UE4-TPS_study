#include "RangedWeaponComponent.h"
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

/*void ATPShooterCharacter::SetupRangedWeaponVariables()
{
	GetWorldTimerManager().ClearTimer(RangedWeaponSetupTimer);

	if (GetRangedWeapon()->WeaponTable != nullptr)
	{
		WeaponNames = GetRangedWeapon()->WeaponTable->GetRowNames();
		GetRangedWeapon()->SetWeaponMode(0);
	}
}*/

void URangedWeaponComponent::SetWeaponMesh()
{
	USkeletalMeshComponent* weaponMesh = Shooter->GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(weaponMesh);
}