#include "RangedWeaponComponent.h"
#include "AmmoAndEnergyComponent.h"
#include "TPS_Projectile.h"
#include "TimerManager.h"

//#include "Blueprint/UserWidget.h"
#include "TPShooterCharacter.h"

#include "Engine/Engine.h"// debug
#include "Kismet/GameplayStatics.h"// debug

//===========================================================================
// public function:
//===========================================================================

URangedWeaponComponent::URangedWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

//=================
// Getter (public):
//=================

//-------------------------------------------------------------

int32 URangedWeaponComponent::GetWeaponIndex() const
{
	return WeaponIndex;
}

int32 URangedWeaponComponent::GetLastWeaponIndex() const
{
	return LastWeaponIndex;
}

FName URangedWeaponComponent::GetWeaponName() const
{
	return WeaponNames[WeaponIndex];
}

//-------------------------------------------------------------

bool URangedWeaponComponent::GetIsTriggerPressed() const
{
	return bIsTriggerPressed;
}

ETriggerMechanism URangedWeaponComponent::GetTriggerMechanism() const
{
	return CurrentWeapon.Trigger;
}

//==================================
// Function for Controller (public):
//==================================

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


//===========================================================================
// protected function:
//===========================================================================

void URangedWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponTable != nullptr)
	{
		WeaponNames = WeaponTable->GetRowNames();
		SetWeaponMode(0);
	}
	else 
	{
		//GEngine->ClearOnScreenDebugMessages();
		UKismetSystemLibrary::PrintString(this, FString("WEAPON TABLE IS EMPTY!!! >O<"), true, false, FLinearColor::Red, 10.0f);
	}

	SetWeaponMesh();
}

//===========================================================================
// private function:
//===========================================================================

void URangedWeaponComponent::SetWeaponIndex(bool isUp)
{
	LastWeaponIndex = WeaponIndex;

	if (Shooter->IsAbleToSwitchWeapon())
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

	if ((WeaponNames.Num() > WeaponIndex) && Shooter->IsAbleToSwitchWeapon())
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

//================
// Fire (private):
//================

void URangedWeaponComponent::SetWeaponMesh()
{	
	USkeletalMeshComponent* weaponMesh = GetCharacter()->GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(weaponMesh);
}

void URangedWeaponComponent::FireAutomaticTrigger()
{
	if (!(bIsTriggerPressed && IsWeaponAbleToFire())) return;

	FireStandardTrigger();
}

bool URangedWeaponComponent::IsWeaponAbleToFire()
{
	return GetIsAiming() && bIsFireRatePassed && IsAmmoEnough();
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
	PlayFireMontage();
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
	switch (AmmoType)
	{
	case EAmmoType::StandardAmmo:
		FireProjectile(&AmmunitionCount.StandardAmmo);
		break;

	case EAmmoType::RifleAmmo:
		FireProjectile(&AmmunitionCount.RifleAmmo);
		break;

	case EAmmoType::ShotgunAmmo:
		FireProjectile(&AmmunitionCount.ShotgunAmmo);
		break;

	case EAmmoType::Rocket:
		FireProjectile(&AmmunitionCount.Rocket);
		break;

	case EAmmoType::Arrow:
		FireProjectile(&AmmunitionCount.Arrow);
		break;

	case EAmmoType::Grenade:
		FireProjectile(&AmmunitionCount.Grenade);
		break;

	case EAmmoType::Mine:
		FireProjectile(&AmmunitionCount.Mine);
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
		FireProjectile(&EnergyExternal.MP);
		break;

	case EEnergyType::Battery:
		FireProjectile(&EnergyExternal.Battery);
		break;

	case EEnergyType::Fuel:
		FireProjectile(&EnergyExternal.Fuel);
		break;

	case EEnergyType::Overheat:
		FireProjectile(&EnergyExternal.Overheat);
		break;

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
			OnNoMoreAmmoDuringMultipleShot.Broadcast(this, i);
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
				OnNoMoreAmmoDuringMultipleShot.Broadcast(this, i);
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
				OnNoMoreAmmoDuringMultipleShot.Broadcast(this, i);
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
	UE_LOG(LogTemp, Log, TEXT("FIRE RATE START!!!!"));

	GetOwner()->GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetOwner()->GetWorldTimerManager().SetTimer(FireRateTimer, this, &URangedWeaponComponent::TimerFireRateReset, CurrentWeapon.FireRateAndOther[0]);
}

void URangedWeaponComponent::TimerFireRateReset()
{
	UE_LOG(LogTemp, Log, TEXT("FIRE RATE TIMER RESERT, IT WORKS OMG!!!!"));
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

void URangedWeaponComponent::FlipOnePressTriggerSwitch()
{
	bOnePressToggle = (bOnePressToggle) ? false : true;
}

FRotator URangedWeaponComponent::GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	//QueryParams.AddIgnoredActor(Shooter);

	FHitResult HitTrace;
	FVector StartTrace = CameraComponent->GetComponentLocation();
	FRotator CameraRotation = CameraComponent->GetComponentRotation();
	FVector LookDirection = UKismetMathLibrary::GetForwardVector(CameraRotation);
	FVector EndTrace = StartTrace + LookDirection * 100.0f * 3000.0f;
	FVector TargetLocation;
	FRotator MuzzleLookRotation;

	if (GetCharacter()->GetWorld()->LineTraceSingleByChannel(HitTrace, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{
		TargetLocation = HitTrace.Location;
		MuzzleLookRotation = UKismetMathLibrary::FindLookAtRotation(SocketTransform.GetLocation(), TargetLocation);
	}

	return MuzzleLookRotation;
}

//=================
// Pickup (public):
//=================

//UE_LOG(LogTemp, Log, TEXT("<<<<<<<<<<<FINISH current time is %f"), CurrentAimingTime);