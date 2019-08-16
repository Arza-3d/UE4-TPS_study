#include "TPS_studyCharacter.h"
#include "Blueprint/UserWidget.h"
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

// 0.a CONSTRUCTION
ATPS_studyCharacter::ATPS_studyCharacter() {
	// basic component setup:
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	RangedWeapon = CreateDefaultSubobject<UTPS_Weapon>(TEXT("RangedWeapon"));
	// basic component setup:
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;
	FollowCamera->bUsePawnControlRotation = false;
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// inherited component setup:
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -97.0f));
	GetMesh()->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	PrimaryActorTick.bStartWithTickEnabled = false;
	// aiming setup:
	if (AimStats.Num() == 0) { AimStats.SetNum(1); };
	AimStats[0].CamBoom.SocketOffset = GetCameraBoom()->SocketOffset;
	AimStats[0].CamBoom.TargetArmLength = GetCameraBoom()->TargetArmLength;
	AimStats[0].CharMov.MaxAcceleration = GetCharacterMovement()->MaxAcceleration;
	AimStats[0].CharMov.MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	AimStats[0].FollCam.FieldOfView = GetFollowCamera()->FieldOfView;
}
void ATPS_studyCharacter::BeginPlay() {
	Super::BeginPlay();
	// aiming timeline setup:
	FOnTimelineFloat onAimingTimeCallback;
	FOnTimelineEventStatic onAimingTimeFinishedCallback;
	if (FloatCurve) {
		AimingTimeline = NewObject<UTimelineComponent>(
			this,
			FName("AimingTimeline")
			);
		AimingTimeline->CreationMethod =
			EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(AimingTimeline);
		AimingTimeline->SetNetAddressable();
		AimingTimeline->SetPropertySetObject(this);
		AimingTimeline->SetDirectionPropertyName(FName("TimeAimingDirection"));
		AimingTimeline->SetLooping(false);
		AimingTimeline->SetTimelineLength(1.0f);
		AimingTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
		AimingTimeline->SetPlaybackPosition(0.0f, false);
		onAimingTimeCallback.BindUFunction(this, FName(TEXT("TimeAiming")));
		onAimingTimeFinishedCallback.BindUFunction(this, FName(TEXT("TimeFinishAiming")));
		AimingTimeline->AddInterpFloat(FloatCurve, onAimingTimeCallback);
		AimingTimeline->SetTimelineFinishedFunc(onAimingTimeFinishedCallback);
		AimingTimeline->RegisterComponent();
	}
	// weapon setup:
	if (WeaponTable != nullptr) { WeaponNames = WeaponTable->GetRowNames(); }
}
void ATPS_studyCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &ATPS_studyCharacter::Aiming);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &ATPS_studyCharacter::AimingStop);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithNumpad_1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithNumpad_2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithNumpad_3);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithNumpad_4);
	PlayerInputComponent->BindAction("ChangeWeaponUp", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithMouseWheel_Up);
	PlayerInputComponent->BindAction("ChangeWeaponDown", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexWithMouseWheel_Down);
	PlayerInputComponent->BindAxis("MoveForward", this, &ATPS_studyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPS_studyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPS_studyCharacter::LookRightAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPS_studyCharacter::LookUpAtRate);
}
void ATPS_studyCharacter::Tick(float DeltaSeconds) {
	Super::Tick(DeltaSeconds);
	if (AimingTimeline) {
		AimingTimeline->TickComponent(DeltaSeconds, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}
// 0.z CONSTRUCTION

// 1.a NAVIGATION
float ATPS_studyCharacter::GetNormalizedForward() { return NormalizedForward; }
float ATPS_studyCharacter::GetNormalizedRight() { return NormalizedRight; }
void ATPS_studyCharacter::MoveForward(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
		if (bIsAiming) {
			bForwardInputPressed = true;
			NormalizedForward = AssignNormalizedVelo(Value, bRightInputPressed);
		}
	} else {
		bForwardInputPressed = false;
		NormalizedForward = 0.0f;
	}
}
void ATPS_studyCharacter::MoveRight(float Value) {
	if ((Controller != NULL) && (Value != 0.0f)) {
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
		if (bIsAiming) {
			bRightInputPressed = true;
			NormalizedRight = AssignNormalizedVelo(Value, bForwardInputPressed);
		}
	} else {
		bRightInputPressed = false;
		NormalizedRight = 0.0f;
	}
}
void ATPS_studyCharacter::LookRightAtRate(float Rate) {
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void ATPS_studyCharacter::LookUpAtRate(float Rate) {
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
// 1.z NAVIGATION

// 2.a AIMING
bool ATPS_studyCharacter::GetIsAiming() { return bIsAiming; }
void ATPS_studyCharacter::Aiming() {
	Aiming_Setup(true);
	bIsTransitioningAiming = true;
	AimingTimeline->Play();
}
void ATPS_studyCharacter::AimingStop() {
	Aiming_Setup(false);
	AimingTimeline->Reverse();
}
void ATPS_studyCharacter::Aiming_Setup(const bool isAiming) {
	bIsAiming = isAiming;
	OrientCharacter(isAiming);
	SetIsTransitioningAiming(isAiming);
	float speed = (isAiming) ? AimingSpeed : StopAimingSpeed;
	AimingTimeline->SetPlayRate(1.0f / speed);
}
void ATPS_studyCharacter::OrientCharacter(bool bMyCharIsAiming) {
	FollowCamera->bUsePawnControlRotation = bMyCharIsAiming;
	bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}
void ATPS_studyCharacter::TimeAiming(float val) {
	float aimingFieldOfView = AimStats[AimStatTargetIndex].FollCam.FieldOfView;
	float aimingMaxAcceleration = AimStats[AimStatTargetIndex].CharMov.MaxAcceleration;
	float aimingTargetArmLength = AimStats[AimStatTargetIndex].CamBoom.TargetArmLength;
	float aimingWalkSpeed = AimStats[AimStatTargetIndex].CharMov.MaxWalkSpeed;
	float defaultFieldOfView = AimStats[AimStatTargetIndex].FollCam.FieldOfView;
	float defaultMaxAcceleration = AimStats[AimStatTargetIndex].CharMov.MaxAcceleration;
	float defaultTargetArmLength = AimStats[AimStatTargetIndex].CamBoom.TargetArmLength;
	float defaultWalkSpeed = AimStats[AimStatTargetIndex].CharMov.MaxWalkSpeed;
	FVector aimingSocketOffset = AimStats[AimStatTargetIndex].CamBoom.SocketOffset;
	FVector defaultSocketOffset = AimStats[AimStatStartIndex].CamBoom.SocketOffset;
	GetCameraBoom()->TargetArmLength = FMath::Lerp(defaultTargetArmLength, aimingTargetArmLength, val);
	GetCameraBoom()->SocketOffset = FMath::Lerp(defaultSocketOffset, aimingSocketOffset, val);
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(defaultWalkSpeed, aimingWalkSpeed, val);
	GetCharacterMovement()->MaxAcceleration = FMath::Lerp(defaultMaxAcceleration, aimingMaxAcceleration, val);
	GetFollowCamera()->SetFieldOfView(FMath::Lerp(defaultFieldOfView, aimingFieldOfView, val));
}
void ATPS_studyCharacter::TimeFinishAiming() { 
	bIsTransitioningAiming = false; 
}
// 2.z AIMING

// 3.a FIRE
bool ATPS_studyCharacter::IsAbleToRepeatAutoFire_Implementation() { return bIsTriggerPressed; }
bool ATPS_studyCharacter::IsCharacterAbleToFire_Implementation() { return !GetCharacterMovement()->IsFalling(); }
bool ATPS_studyCharacter::IsEnoughForWeaponCost() {
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
bool ATPS_studyCharacter::IsWeaponAbleToFire() { return IsEnoughForWeaponCost() && bIsFireRatePassed; }
bool ATPS_studyCharacter::GetIsTriggerPressed() { return bIsTriggerPressed; }
float ATPS_studyCharacter::GetNewPlayRateForMontage(float targetDuration, UAnimMontage* animMontage) {
	return (targetDuration <= 0.0f) ? 1.0f : animMontage->SequenceLength / targetDuration;
}
FRotator ATPS_studyCharacter::GetNewMuzzleRotation(FTransform socketTransform)
{
	return FRotator();
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
void ATPS_studyCharacter::Fire_Base(bool isTriggerPressed) {
	bIsTriggerPressed = isTriggerPressed;
	if (!bIsAiming) { return; }
	switch (CurrentWeapon.Trigger) {
	case ETriggerMechanism::PressTrigger:
		Fire__Standard(isTriggerPressed);
		break;
	case ETriggerMechanism::AutomaticTrigger:
		Fire__Automatic(isTriggerPressed);
		break;
	case ETriggerMechanism::ReleaseTrigger:
		Fire__HoldRelease(isTriggerPressed);
		break;
	case ETriggerMechanism::OnePressAutoTrigger:
		Fire__AutomaticOnePress(isTriggerPressed);
		break;
	default:
		Fire__Standard(isTriggerPressed);
	}
}
void ATPS_studyCharacter::Fire__Automatic(bool pressed) {
	if (!pressed) { return; }
	SpawnProjectile(GetMesh());
}
void ATPS_studyCharacter::Fire__AutomaticOnePress(bool pressed) {
}
void ATPS_studyCharacter::Fire__HoldRelease(bool pressed) {
}
void ATPS_studyCharacter::Fire__Standard(bool pressed) {
	if (!pressed) { return; }
	TimerFireRate_Start();
	//FTransform socketTransform
	//Fire();
}
void ATPS_studyCharacter::PlayFireMontage() {
	UAnimMontage* fireMontage = ShooterState.CharacterWeaponMontage[0];
	if (fireMontage) {
		float playRate = GetNewPlayRateForMontage(CurrentWeapon.FireRateAndOther[0], fireMontage);
		PlayAnimMontage(fireMontage, playRate);
	}
}
void ATPS_studyCharacter::SpawnProjectile(USkeletalMeshComponent* weaponMesh) {
	UWorld* world = GetWorld();
	USceneComponent* weaponInWorld = Cast<USceneComponent>(weaponMesh);
	TArray<FName> muzzleName = CurrentWeapon.SocketName;
	for (int i = 0; i < muzzleName.Num(); i++) {
		if (IsNoMoreAmmo()) { break; }
		ConsumeWeaponCost();
		FTransform muzzleTransform = weaponInWorld->GetSocketTransform(muzzleName[i]);
		FTransform spawnTransform = FTransform(
			GetNewMuzzleRotation(muzzleTransform),
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
void ATPS_studyCharacter::TimerFireRate_Start() {
	bIsFireRatePassed = false;
	GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetWorldTimerManager().SetTimer(FireRateTimer, this,
		&ATPS_studyCharacter::TimerFireRate_Reset, CurrentWeapon.FireRateAndOther[0]
	);
}
void ATPS_studyCharacter::TimerFireRate_Reset() {
	bIsFireRatePassed = true;
	GetWorldTimerManager().ClearTimer(FireRateTimer);
}

// 3.z FIRE

// 4.a SWITCH WEAPON
bool ATPS_studyCharacter::IsSwitchWeaponRequirementFulfilled_Implementation() {
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetIsAiming();
	return bIsOnTheGround && bIsNotAiming;
}
int ATPS_studyCharacter::GetLastWeaponIndex() { return LastWeaponIndex; }
int ATPS_studyCharacter::GetWeaponIndex() { return WeaponIndex; }
void ATPS_studyCharacter::SetWeaponMode(int weaponIndex) {
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
void ATPS_studyCharacter::SetWeaponIndexWithMouseWheel(bool isUp) {
	LastWeaponIndex = WeaponIndex;
	if (IsSwitchWeaponRequirementFulfilled()) {
		int counter = isUp ? 1 : -1;
		int withinRange = (WeaponIndex + counter) % WeaponNames.Num();
		WeaponIndex = (withinRange >= 0) ? withinRange : WeaponNames.Num() - 1;
		OnSwitchWeaponSuccess();
	}
}
void ATPS_studyCharacter::SetWeaponIndexWithMouseWheel_Up() { SetWeaponIndexWithMouseWheel(true); }
void ATPS_studyCharacter::SetWeaponIndexWithMouseWheel_Down() { SetWeaponIndexWithMouseWheel(false); }
void ATPS_studyCharacter::SetWeaponIndexWithNumpad(int numberInput) {
	if (numberInput >= WeaponNames.Num()) { return; }
	LastWeaponIndex = WeaponIndex;
	if ((WeaponNames.Num() > WeaponIndex) && IsSwitchWeaponRequirementFulfilled()) {
		WeaponIndex = numberInput;
	}
	OnSwitchWeaponSuccess();
}
void ATPS_studyCharacter::SetWeaponIndexWithNumpad_1() { SetWeaponIndexWithNumpad(0); }
void ATPS_studyCharacter::SetWeaponIndexWithNumpad_2() { SetWeaponIndexWithNumpad(1); }
void ATPS_studyCharacter::SetWeaponIndexWithNumpad_3() { SetWeaponIndexWithNumpad(2); }
void ATPS_studyCharacter::SetWeaponIndexWithNumpad_4() { SetWeaponIndexWithNumpad(3); }
// 4.z SWITCH WEAPON

// 5.a PICKUP
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
// 5.z PICKUP

void ATPS_studyCharacter::SetIsTransitioningAiming(bool isTransitioningAiming) {
	bIsTransitioningAiming = isTransitioningAiming;
}
bool ATPS_studyCharacter::GetTransitioningAiming() { return bIsTransitioningAiming; }
float ATPS_studyCharacter::AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed) {
	FVector myVelo = GetVelocity();
	float mySpeed = FVector(myVelo.X, myVelo.Y, 0.0f).Size();
	float divider = (bOtherButtonPressed) ? UKismetMathLibrary::Sqrt(2.0f) : 1.0f;
	return (mySpeed * MyValue) / (divider * GetCharacterMovement()->MaxWalkSpeed);
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
bool ATPS_studyCharacter::IsNotOverheat() {
	return true; //WeaponTemperature < temperatureLimit;
}
bool ATPS_studyCharacter::IsEnergyEnough() {
	return true;
}
void ATPS_studyCharacter::SetProjectileMultiplier(float projectileMultiplier) { ProjectileMultiplier = projectileMultiplier; }
float ATPS_studyCharacter::GetProjectileMultipler() { return ProjectileMultiplier; }
bool ATPS_studyCharacter::GetIsFireRatePassed() { return bIsFireRatePassed; }
void ATPS_studyCharacter::SetIsFireRatePassed(bool bFireRatePassed) { bIsFireRatePassed = bFireRatePassed; }
