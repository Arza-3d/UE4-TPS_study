#include "TPS_studyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/DataTable.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "TPSAnimInterface.h"
#include "TPS_Projectile.h"
#include "TPS_Weapon.h"

//#include "Components/TimelineComponent.h"

ATPS_studyCharacter::ATPS_studyCharacter()
{
	PrimaryActorTick.bStartWithTickEnabled = false;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false;

	RangedWeapon = CreateDefaultSubobject<UTPS_Weapon>(TEXT("RangedWeapon"));

	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));

	bIsFireRatePassed = true;

	ProjectileMultiplier = 1.0f;
	AimingSpeed = 1.0f;
	StopAimingSpeed = 1.0f;

}

void ATPS_studyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ATPS_studyCharacter::SetWeapon1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ATPS_studyCharacter::SetWeapon2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ATPS_studyCharacter::SetWeapon3);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ATPS_studyCharacter::SetWeapon4);

	PlayerInputComponent->BindAction("ChangeWeaponUp", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponUp);
	PlayerInputComponent->BindAction("ChangeWeaponDown", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponDown);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPS_studyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPS_studyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPS_studyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPS_studyCharacter::LookUpAtRate);
}

void ATPS_studyCharacter::BeginPlay() {
	Super::BeginPlay();
	if (WeaponTable != nullptr) { WeaponNames = WeaponTable->GetRowNames(); }
}
void ATPS_studyCharacter::SetIsTransitioningAiming(bool isTransitioningAiming) { 
	bIsTransitioningAiming = isTransitioningAiming;
}
bool ATPS_studyCharacter::GetTransitioningAiming() { return bIsTransitioningAiming; }

void ATPS_studyCharacter::SetWeapon1() { SetWeaponIndexWithNumpad(0); } 
void ATPS_studyCharacter::SetWeapon2() { SetWeaponIndexWithNumpad(1); }
void ATPS_studyCharacter::SetWeapon3() { SetWeaponIndexWithNumpad(2); }
void ATPS_studyCharacter::SetWeapon4() { SetWeaponIndexWithNumpad(3); }

void ATPS_studyCharacter::SetWeaponUp() { SetWeaponIndexWithMouseWheel(true); }
void ATPS_studyCharacter::SetWeaponDown() { SetWeaponIndexWithMouseWheel(false); }

void ATPS_studyCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPS_studyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

float ATPS_studyCharacter::AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed)
{
	FVector myVelo = GetVelocity();

	float mySpeed = FVector(myVelo.X, myVelo.Y, 0.0f).Size();
	float divider = (bOtherButtonPressed) ? UKismetMathLibrary::Sqrt(2.0f) : 1.0f;
	
	return 
		(mySpeed * MyValue) 
		/ 
		(divider * GetCharacterMovement()->MaxWalkSpeed);
}

bool ATPS_studyCharacter::CanWeaponFire()
{
	return IsEnoughForWeaponCost() && bIsFireRatePassed;
}

bool ATPS_studyCharacter::IsEnoughForWeaponCost()
{
	if (CurrentWeapon.WeaponCost == EWeaponCost::Nothing) { return true; }

	switch (CurrentWeapon.WeaponCost) {
	case EWeaponCost::Ammo:
		return IsAmmoEnough(CurrentWeapon.AmmoType);
	case EWeaponCost::Energy:
		return IsEnergyEnough();
	case EWeaponCost::Overheat:
		return IsNotOverheat();
	default:
		return false;
	}
}

bool ATPS_studyCharacter::CheckAndCallRunOutOfAmmo(int ammo) {
	if (ammo <= 0) { OnWeaponRunOutOfAmmo(); }
	return ammo > 0;
}
bool ATPS_studyCharacter::IsAmmoEnough(EAmmoType ammo) {
	switch (ammo) {
	case EAmmoType::StandardAmmo:
		return CheckAndCallRunOutOfAmmo(Ammunition.StandardAmmo);
	case EAmmoType::RifleAmmo:
		return CheckAndCallRunOutOfAmmo(Ammunition.RifleAmmo);
	case EAmmoType::ShotgunAmmo:
		return CheckAndCallRunOutOfAmmo(Ammunition.ShotgunAmmo);
	case EAmmoType::Rocket:
		return CheckAndCallRunOutOfAmmo(Ammunition.Rocket);
	case EAmmoType::Arrow:
		return CheckAndCallRunOutOfAmmo(Ammunition.Arrow);
	case EAmmoType::Grenade:
		return CheckAndCallRunOutOfAmmo(Ammunition.Grenade);
	case EAmmoType::Mine:
		return CheckAndCallRunOutOfAmmo(Ammunition.Mine);
	default:
		return false;
	}
}

bool ATPS_studyCharacter::IsNotOverheat()
{
	return true; //WeaponTemperature < temperatureLimit;
}

bool ATPS_studyCharacter::IsEnergyEnough()
{
	return true;
}

void ATPS_studyCharacter::StartFireRateCount()
{
	bIsFireRatePassed = false;
	GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetWorldTimerManager().SetTimer(FireRateTimer, this,
		&ATPS_studyCharacter::ResetFireRateCount, CurrentWeapon.FireRateAndOther[0]
	);
}

void ATPS_studyCharacter::ResetFireRateCount()
{
	bIsFireRatePassed = true;
	GetWorldTimerManager().ClearTimer(FireRateTimer);
}

int ATPS_studyCharacter::GetWeaponIndex()
{
	return WeaponIndex;
}

int ATPS_studyCharacter::GetLastWeaponIndex()
{
	return LastWeaponIndex;
}

void ATPS_studyCharacter::SetProjectileMultiplier(float projectileMultiplier)
{
	ProjectileMultiplier = projectileMultiplier;
}

float ATPS_studyCharacter::GetProjectileMultipler()
{
	return ProjectileMultiplier;
}

void ATPS_studyCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		if (bIsAiming) {
			bForwardInputPressed = true;
			NormalizedForward = AssignNormalizedVelo(Value, bRightInputPressed);			
		}
	}
	else
	{
		bForwardInputPressed = false;
		NormalizedForward = 0.0f;
	}
}

void ATPS_studyCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		if (bIsAiming) {
			bRightInputPressed = true;
			NormalizedRight = AssignNormalizedVelo(Value, bForwardInputPressed);
		}
	}
	else 
	{
		bRightInputPressed = false;
		NormalizedRight = 0.0f;
	}
}

bool ATPS_studyCharacter::GetIsFireRatePassed()
{
	return bIsFireRatePassed;
}

void ATPS_studyCharacter::SetIsFireRatePassed(bool bFireRatePassed)
{
	bIsFireRatePassed = bFireRatePassed;
}

void ATPS_studyCharacter::Aiming(bool bIsCharAiming) {
	bIsAiming = bIsCharAiming;
	OrientCharacter(bIsCharAiming);
}

void ATPS_studyCharacter::SetWeaponIndexWithNumpad(int numberInput)
{
	if (numberInput >= WeaponNames.Num()) { return; }
	LastWeaponIndex = WeaponIndex;

	if ((WeaponNames.Num() > WeaponIndex) && IsSwitchWeaponRequirementFulfilled())
	{
		WeaponIndex = numberInput;
	}
	OnSwitchWeaponSuccess();
}

void ATPS_studyCharacter::SetWeaponIndexWithMouseWheel(bool isUp) {
	LastWeaponIndex = WeaponIndex;
	if (IsSwitchWeaponRequirementFulfilled()) {
		int counter = isUp ? 1 : -1;
		int withinRange = (WeaponIndex + counter) % WeaponNames.Num();
		if(withinRange >= 0) {
			WeaponIndex = withinRange;
		} else {
			WeaponIndex = WeaponNames.Num() - 1;
		}
		OnSwitchWeaponSuccess();
	}
}



bool ATPS_studyCharacter::IsAbleToRepeatAutoFire_Implementation() {
	return bIsTriggerPressed;
}

bool ATPS_studyCharacter::IsSwitchWeaponRequirementFulfilled_Implementation() {
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetIsAiming();
	return bIsOnTheGround && bIsNotAiming;
}





void ATPS_studyCharacter::GetCurrentWeaponMode(int weaponIndex) {
	FName currentWeaponName = WeaponNames[weaponIndex];
	static const FString contextString(TEXT("Weapon Mode"));
	struct FWeaponModeCompact* weaponModeRow;
	weaponModeRow = WeaponModeTable->FindRow<FWeaponModeCompact>(currentWeaponName, contextString, true);
	if (weaponModeRow) {
		FWeaponMode currentWeaponMode = weaponModeRow->WeaponMode;
		ShooterState = currentWeaponMode.Shooter;
		CurrentWeapon = currentWeaponMode.Weapon;
		CurrentProjectile = currentWeaponMode.Projectile;
	}
}


void ATPS_studyCharacter::SpawnProjectile(USkeletalMeshComponent* weaponMesh) {
	UWorld* world = GetWorld();
	USceneComponent* weaponInWorld = Cast<USceneComponent>(weaponMesh);
	TArray<FName> muzzleName = CurrentWeapon.SocketName;
	for (int i = 0; i < muzzleName.Num(); i++) {
		if (IsNoMoreAmmo()) { break; }
		ConsumeWeaponCost();
		//FTransform spawnTransform;
		FTransform muzzleTransform = weaponInWorld->GetSocketTransform(muzzleName[i]);
		FTransform spawnTransform = FTransform(
			FixMuzzleRotation(muzzleTransform),
			muzzleTransform.GetLocation(),
			muzzleTransform.GetScale3D()
		);
		ATPS_Projectile* myProjectile = world->SpawnActorDeferred<ATPS_Projectile>(
			ATPS_Projectile::StaticClass(), 
			spawnTransform
		);
		myProjectile->SetUpProjectile(CurrentProjectile);
		myProjectile->FinishSpawning(spawnTransform);
	}
}





void ATPS_studyCharacter::AutomaticFire(bool pressed)
{
	if (!pressed) {return;}

	SpawnProjectile(GetMesh());
}

void ATPS_studyCharacter::HoldReleaseFire(bool pressed)
{
}

void ATPS_studyCharacter::OnePressAutoFire(bool pressed)
{
}

// 1.a NAVIGATION
float ATPS_studyCharacter::GetNormalizedForward() { return NormalizedForward; }
float ATPS_studyCharacter::GetNormalizedRight() { return NormalizedRight; }
// 1.z NAVIGATION

// 2.a AIMING
bool ATPS_studyCharacter::GetIsAiming() { return bIsAiming; }
void ATPS_studyCharacter::OrientCharacter(bool bMyCharIsAiming) {
	FollowCamera->bUsePawnControlRotation = bMyCharIsAiming;
	bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}
// 2.z AIMING

// 3.a FIRE
bool ATPS_studyCharacter::CanCharacterFire_Implementation() { return !GetCharacterMovement()->IsFalling(); }
bool ATPS_studyCharacter::GetIsTriggerPressed() { return bIsTriggerPressed; }
bool ATPS_studyCharacter::IsNoMoreAmmo() {
	switch (CurrentWeapon.AmmoType) {
	case EAmmoType::StandardAmmo:
		return Ammunition.StandardAmmo <= 0;
	case EAmmoType::RifleAmmo:
		return Ammunition.RifleAmmo <= 0;
	case EAmmoType::ShotgunAmmo:
		return Ammunition.ShotgunAmmo <= 0;
	case EAmmoType::Rocket:
		return Ammunition.Rocket <= 0;
	case EAmmoType::Arrow:
		return Ammunition.Arrow <= 0;
	case EAmmoType::Grenade:
		return Ammunition.Grenade <= 0;
	case EAmmoType::Mine:
		return Ammunition.Mine <= 0;
	default:
		return false;
	}
}
float ATPS_studyCharacter::GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage) {
	return (targetDuration <= 0.0f) ? 1.0f : animMontage->SequenceLength / targetDuration;
}
FRotator ATPS_studyCharacter::FixMuzzleRotation(FTransform socketTransform)
{
	return FRotator();
}
void ATPS_studyCharacter::CharacterPlayMontage() {
	UAnimMontage* fireMontage = ShooterState.CharacterWeaponMontage[0];
	if (fireMontage) {
		float playRate = GetNewPlayRateForMontage(CurrentWeapon.FireRateAndOther[0], fireMontage);
		PlayAnimMontage(fireMontage, playRate);
	}
}
void ATPS_studyCharacter::ConsumeWeaponCost() {
	switch (CurrentWeapon.AmmoType) {
	case EAmmoType::StandardAmmo:
		Ammunition.StandardAmmo--;
		break;
	case EAmmoType::RifleAmmo:
		Ammunition.RifleAmmo--;
		break;
	case EAmmoType::ShotgunAmmo:
		Ammunition.ShotgunAmmo--;
		break;
	case EAmmoType::Rocket:
		Ammunition.Rocket--;
		break;
	case EAmmoType::Arrow:
		Ammunition.Arrow--;
		break;
	case EAmmoType::Grenade:
		Ammunition.Grenade--;
		break;
	case EAmmoType::Mine:
		Ammunition.Mine--;
		break;
	default:
		break;
	}
}
void ATPS_studyCharacter::MainFire(bool isTriggerPressed) {
	bIsTriggerPressed = isTriggerPressed;
	if (!bIsAiming) { return; }
	switch (CurrentWeapon.Trigger) {
	case ETriggerMechanism::PressTrigger:
		StandardFire(isTriggerPressed);
		break;
	case ETriggerMechanism::AutomaticTrigger:
		AutomaticFire(isTriggerPressed);
		break;
	case ETriggerMechanism::ReleaseTrigger:
		HoldReleaseFire(isTriggerPressed);
		break;
	case ETriggerMechanism::OnePressAutoTrigger:
		OnePressAutoFire(isTriggerPressed);
		break;
	default:
		StandardFire(isTriggerPressed);
	}
}
void ATPS_studyCharacter::StandardFire(bool pressed) {
	if (!pressed) { return; }
	StartFireRateCount();
	//FTransform socketTransform
	//Fire();
}
// 3.z FIRE

// 4.a PICKUP
void ATPS_studyCharacter::AddAmmo(int addAmmo, EAmmoType ammoType) {
	switch (ammoType) {
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
// 4.z PICKUP
