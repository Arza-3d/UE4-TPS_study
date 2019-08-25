#include "TPShooterCharacter.h"
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
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "RangedWeaponComponent.h"
#include "TimerManager.h"
#include "TPSAnimInterface.h"
#include "TPSFunctionLibrary.h"
#include "TPS_Projectile.h"
#include "DrawDebugHelpers.h"

// 0.a CONSTRUCTION
ATPShooterCharacter::ATPShooterCharacter()
{
	// basic component setup:
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	RangedWeapon = CreateDefaultSubobject<URangedWeaponComponent>(TEXT("RangedWeapon"));

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

	// fire setup:
	SetWeaponMesh();
}

/*void ATPShooterCharacter::ChangeControl()
{
	PlayerInputComponent->BindAction("AttackAction", IE_Pressed, this, &ATPShooterCharacter::FirePress);
}*/

float ATPShooterCharacter::GetHP() const
{
	return CharacterStat.HP;
}

void ATPShooterCharacter::SetHP(float val)
{
	CharacterStat.HP = val;
}

float ATPShooterCharacter::GetMP() const
{
	return CharacterStat.MP;
}

void ATPShooterCharacter::SetMP(float val)
{
	CharacterStat.MP = val;
}

void ATPShooterCharacter::BeginPlay() {
	Super::BeginPlay();

	// aiming setup:
	if (GetRangedWeapon()->AimingTable != nullptr)
	{
		AimingNames = GetRangedWeapon()->AimingTable->GetRowNames();
	}

	int aimingNamesCount = AimingNames.Num();
	FName currentAimingName;
	static const FString contextString(TEXT("Aiming name"));
	struct FAimingStatCompact* aimStatRow;
	AimStats.SetNum(1 + aimingNamesCount);

	for (int i = 0; i < AimingNames.Num(); i++)
	{
		currentAimingName = AimingNames[i];
		aimStatRow = GetRangedWeapon()->AimingTable->FindRow<FAimingStatCompact>(currentAimingName, contextString, true);
		AimStats[1 + i].CamBoom = aimStatRow->AimStat.CamBoom;
		AimStats[1 + i].CharMov = aimStatRow->AimStat.CharMov;
		AimStats[1 + i].FollCam = aimStatRow->AimStat.FollCam;
	}

	// aiming timeline setup:
	FOnTimelineFloat onAimingTimeCallback;
	FOnTimelineEventStatic onAimingTimeFinishedCallback;

	if (FloatCurve)
	{
		AimingTimeline = NewObject<UTimelineComponent>(this, FName("AimingTimeline"));
		AimingTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
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
	//GetWorldTimerManager().ClearTimer(RangedWeaponSetupTimer);
	//GetWorldTimerManager().SetTimer(RangedWeaponSetupTimer, this, &ATPShooterCharacter::SetupRangedWeaponVariables, 0.3f);
}

void ATPShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("AttackAction", IE_Pressed, this, &ATPShooterCharacter::FirePress);
	PlayerInputComponent->BindAction("AttackAction", IE_Released, this, &ATPShooterCharacter::FireRelease);
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &ATPShooterCharacter::AimingPress);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &ATPShooterCharacter::AimingRelease);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndex1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndex2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndex3);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndex4);
	PlayerInputComponent->BindAction("ChangeWeaponUp", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndexUp);
	PlayerInputComponent->BindAction("ChangeWeaponDown", IE_Pressed, this, &ATPShooterCharacter::SetWeaponIndexDown);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPShooterCharacter::LookRightAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPShooterCharacter::LookUpAtRate);
}

void ATPShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AimingTimeline)
	{
		AimingTimeline->TickComponent(DeltaSeconds, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}
// 0.z CONSTRUCTION

// 1.a NAVIGATION
float ATPShooterCharacter::GetNormalizedForward() const
{
	return NormalizedForward;
}

float ATPShooterCharacter::GetNormalizedRight() const
{
	return NormalizedRight;
}

ETriggerMechanism ATPShooterCharacter::GetTriggerMechanism() const
{
	return CurrentWeapon.Trigger;
}

/*FName ATPShooterCharacter::GetWeaponName() const
{
	return WeaponNames[GetRangedWeapon()->WeaponIndex];
}*/

void ATPShooterCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		if (bIsAiming)
		{
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

void ATPShooterCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);

		if (bIsAiming)
		{
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

void ATPShooterCharacter::LookRightAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPShooterCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
// 1.z NAVIGATION

// 2.a AIMING
bool ATPShooterCharacter::GetIsAiming() const
{
	return (bWeaponIsAlwaysAiming) ? true : bIsAiming;
}

void ATPShooterCharacter::AimingPress() {

	if (!IsAbleToAim()) { return; }

	Aiming(true);

	OnAiming();

	bIsTransitioningAiming = true;

	AimingTimeline->Play();
}

void ATPShooterCharacter::AimingRelease()
{
	Aiming(false);

	AimingTimeline->Reverse();

	OnStopAiming();

	bOnePressToggle = false;

	if (CurrentWeapon.Trigger == ETriggerMechanism::ReleaseTrigger)
	{
		GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
		bMaxHoldIsReach = false;
		HoldTime = 0.0f;
	}
}

void ATPShooterCharacter::Aiming(const bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	OrientCharacter(bInIsAiming);
	bIsTransitioningAiming = bInIsAiming;

	float speed = (bInIsAiming) ? GetRangedWeapon()->AimingSpeed : GetRangedWeapon()->StopAimingSpeed;

	if (speed <= 0.0f) { speed = 1.0f; };

	AimingTimeline->SetPlayRate(1.0f / speed);
}

void ATPShooterCharacter::OrientCharacter(bool bMyCharIsAiming)
{
	FollowCamera->bUsePawnControlRotation = bMyCharIsAiming;
	bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}

void ATPShooterCharacter::TimeAiming(float InAlpha)
{
	int32 A = AimStatStartIndex;
	int32 B = AimStatTargetIndex;

	float defaultFieldOfView = AimStats[A].FollCam.FieldOfView;
	float defaultMaxAcceleration = AimStats[A].CharMov.MaxAcceleration;
	float defaultTargetArmLength = AimStats[A].CamBoom.TargetArmLength;
	float defaultWalkSpeed = AimStats[A].CharMov.MaxWalkSpeed;
	FVector defaultSocketOffset = AimStats[A].CamBoom.SocketOffset;

	float aimingFieldOfView = AimStats[B].FollCam.FieldOfView;
	float aimingMaxAcceleration = AimStats[B].CharMov.MaxAcceleration;
	float aimingTargetArmLength = AimStats[B].CamBoom.TargetArmLength;
	float aimingWalkSpeed = AimStats[B].CharMov.MaxWalkSpeed;
	FVector aimingSocketOffset = AimStats[B].CamBoom.SocketOffset;

	GetCameraBoom()->TargetArmLength = FMath::Lerp(defaultTargetArmLength, aimingTargetArmLength, InAlpha);
	GetCameraBoom()->SocketOffset = FMath::Lerp(defaultSocketOffset, aimingSocketOffset, InAlpha);
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(defaultWalkSpeed, aimingWalkSpeed, InAlpha);
	GetCharacterMovement()->MaxAcceleration = FMath::Lerp(defaultMaxAcceleration, aimingMaxAcceleration, InAlpha);
	GetFollowCamera()->SetFieldOfView(FMath::Lerp(defaultFieldOfView, aimingFieldOfView, InAlpha));
}

void ATPShooterCharacter::TimeFinishAiming()
{
	bIsTransitioningAiming = false;
	OnAiming();
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
// 2.z AIMING

// 3.a FIRE
bool ATPShooterCharacter::IsAbleToRepeatAutoFire_Implementation()
{
	return bIsTriggerPressed;
}

bool ATPShooterCharacter::IsAbleToFire_Implementation()
{
	return !GetCharacterMovement()->IsFalling();
}

bool ATPShooterCharacter::IsAmmoEnough()
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

bool ATPShooterCharacter::GetIsTriggerPressed() const
{
	return bIsTriggerPressed;
}

FRotator ATPShooterCharacter::GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform)
{
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(this);

	FHitResult HitTrace;
	FVector StartTrace = GetFollowCamera()->GetComponentLocation();
	FRotator CameraRotation = GetFollowCamera()->GetComponentRotation();
	FVector LookDirection = UKismetMathLibrary::GetForwardVector(CameraRotation);
	FVector EndTrace = StartTrace + LookDirection * 100.0f * 3000.0f;
	FVector TargetLocation;
	FRotator MuzzleLookRotation;

	if (GetWorld()->LineTraceSingleByChannel(HitTrace, StartTrace, EndTrace, ECC_Visibility, QueryParams))
	{
		TargetLocation = HitTrace.Location;
		MuzzleLookRotation = UKismetMathLibrary::FindLookAtRotation(SocketTransform.GetLocation(), TargetLocation);
	}

	return MuzzleLookRotation;
}

void ATPShooterCharacter::FirePress()
{
	bIsTriggerPressed = true;
	FlipOnePressTriggerSwitch();

	if (!(IsWeaponAbleToFire())) { return; }

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

void ATPShooterCharacter::FireRelease()
{
	bIsTriggerPressed = false;

	if (CurrentWeapon.Trigger == ETriggerMechanism::ReleaseTrigger)
	{
		FireReleaseAfterHold();
	}
}

void ATPShooterCharacter::FireAutomaticTrigger()
{
	if (!(bIsTriggerPressed && IsWeaponAbleToFire()))
	{
		return;
	}

	FireStandardTrigger();
}

bool ATPShooterCharacter::IsWeaponAbleToFire()
{
	return GetIsAiming() && bIsFireRatePassed && IsAmmoEnough();
}

void ATPShooterCharacter::FireAutomaticTriggerOnePress()
{
	if (bOnePressToggle && IsWeaponAbleToFire())
	{
		FireStandardTrigger();
	}
}

void ATPShooterCharacter::FireHold()
{
	GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	GetWorldTimerManager().SetTimer(TimerOfHoldTrigger, this, &ATPShooterCharacter::CountHoldTriggerTime, HoldTimeRateCount, true);
}

void ATPShooterCharacter::CountHoldTriggerTime()
{
	HoldTime += HoldTimeRateCount;

	if (!bMaxHoldIsReach)
	{
		if (HoldTime >= MaxFireHoldTime)
		{
			bMaxHoldIsReach = true;
			OnMaxFireHold();
		}
	}
}

void ATPShooterCharacter::FireReleaseAfterHold()
{
	GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);

	if (bMaxHoldIsReach)
	{
		FireStandardTrigger();
		OnMaxFireHoldRelease();
	}
	else if (HoldTime >= CurrentWeapon.FireRateAndOther[0])
	{
		FireStandardTrigger();
	}

	bMaxHoldIsReach = false;
	HoldTime = 0.0f;
}

void ATPShooterCharacter::FireStandardTrigger()
{
	TimerFireRateStart();
	PlayFireMontage();
	OnWeaponFires();

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

void ATPShooterCharacter::FireProjectile(const EAmmoType AmmoType)
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

void ATPShooterCharacter::FireProjectile(const EEnergyType EnergyType)
{
	switch (EnergyType)
	{
	case EEnergyType::MP:
		FireProjectile(&CharacterStat.MP);
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

void ATPShooterCharacter::FireProjectile()
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int MuzzleCount = MuzzleName.Num();

	for (int i = 0; i < MuzzleCount; i++)
	{
		SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
	}
}

void ATPShooterCharacter::FireProjectile(int* Ammo)
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int32 MuzzleCount = MuzzleName.Num();
	int32 CurrentAmmo = *Ammo;

	for (int i = 0; i < MuzzleCount; i++)
	{
		if (CurrentAmmo <= 0) {
			OnNoMoreAmmoDuringFire();
			break;
		}

		CurrentAmmo--;
		SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
	}

	*Ammo = CurrentAmmo;
}

void ATPShooterCharacter::FireProjectile(float* MyEnergy)
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
				OnNoMoreAmmoDuringFire();
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
				OnNoMoreAmmoDuringFire();
				break;
			}

			CurrentEnergy += EnergyCostPerShot;
			SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
		}
	}
	*MyEnergy = CurrentEnergy;
}

void ATPShooterCharacter::SpawnProjectile(USceneComponent* MyWeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int32 i)
{
	FTransform MuzzleTransform = MyWeaponInWorld->GetSocketTransform(MuzzleName[i]);
	FTransform SpawnTransform = FTransform(GetNewMuzzleRotationFromLineTrace(MuzzleTransform), MuzzleTransform.GetLocation(), MuzzleTransform.GetScale3D());

	ATPS_Projectile* MyProjectile = MyWorld->SpawnActorDeferred<ATPS_Projectile>(ATPS_Projectile::StaticClass(), SpawnTransform);

	MyProjectile->SetUpProjectile(CurrentProjectile);
	MyProjectile->FinishSpawning(SpawnTransform);
}

void ATPShooterCharacter::PlayFireMontage()
{
	UAnimMontage* fireMontage;

	if (ShooterState.CharacterWeaponMontage.Num() > 0)
	{
		fireMontage = ShooterState.CharacterWeaponMontage[0];

		if (fireMontage)
		{
			float playRate = UTPSFunctionLibrary::GetNewPlayRateForMontage(CurrentWeapon.FireRateAndOther[0], fireMontage);
			PlayAnimMontage(fireMontage, playRate);
		}
	}
}

void ATPShooterCharacter::TimerFireRateStart()
{
	bIsFireRatePassed = false;

	GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetWorldTimerManager().SetTimer(FireRateTimer, this, &ATPShooterCharacter::TimerFireRateReset, CurrentWeapon.FireRateAndOther[0]);
}

void ATPShooterCharacter::TimerFireRateReset()
{
	bIsFireRatePassed = true;
	GetWorldTimerManager().ClearTimer(FireRateTimer);

	if (CurrentWeapon.Trigger == ETriggerMechanism::AutomaticTrigger)
	{
		FireAutomaticTrigger();
	}
	else if (CurrentWeapon.Trigger == ETriggerMechanism::OnePressAutoTrigger)
	{
		FireAutomaticTriggerOnePress();
	}
}

bool ATPShooterCharacter::IsAmmoEnough(const int32 InAmmo)
{
	bool bAmmoIsEmpty = InAmmo <= 0;

	if (bAmmoIsEmpty)
	{
		OnNoAmmo();
	}

	return !bAmmoIsEmpty;
}

bool ATPShooterCharacter::IsAmmoEnough(const EAmmoType Ammo)
{
	switch (Ammo)
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

bool ATPShooterCharacter::IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot)
{
	bool bIsNotEnoughEnergy = MyEnergy < MyEnergyPerShot;

	if (bIsNotEnoughEnergy)
	{
		OnNoEnergy();
	}

	return !bIsNotEnoughEnergy;
}

bool ATPShooterCharacter::IsAmmoEnough(const EEnergyType EnergyType)
{
	switch (CurrentWeapon.EnergyType)
	{
	case EEnergyType::MP:
		return IsAmmoEnough(CharacterStat.MP, CurrentWeapon.EnergyUsePerShot);

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

bool ATPShooterCharacter::IsWeaponNotOverheating()
{
	bool bIsOverheat = EnergyExternal.Overheat >= 100.0f;

	if (bIsOverheat)
	{
		OnWeaponOverheats();
	}

	return !bIsOverheat;
}
// 3.z FIRE

// 4.a SWITCH WEAPON

bool ATPShooterCharacter::IsAbleToSwitchWeapon_Implementation()
{
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetIsAiming();

	return bIsOnTheGround && bIsNotAiming;
}

bool ATPShooterCharacter::IsAbleToAim_Implementation()
{
	return !GetCharacterMovement()->IsFalling();
}

/*int32 ATPShooterCharacter::GetLastWeaponIndex() const
{
	return GetRangedWeapon()->LastWeaponIndex;
}

int32 ATPShooterCharacter::GetWeaponIndex() const
{
	return GetRangedWeapon()->WeaponIndex;
}*/

/*void ATPShooterCharacter::SetWeaponMode(const int32 MyWeaponIndex)
{
	FName CurrentWeaponName = WeaponNames[MyWeaponIndex];
	static const FString ContextString(TEXT("Weapon Mode"));
	struct FWeaponModeCompact* WeaponModeRow;
	WeaponModeRow = GetRangedWeapon()->WeaponTable->FindRow<FWeaponModeCompact>(CurrentWeaponName, ContextString, true);

	if (WeaponModeRow)
	{
		FWeaponMode CurrentWeaponMode = WeaponModeRow->WeaponMode;

		ShooterState = CurrentWeaponMode.Shooter;
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
}*/

void ATPShooterCharacter::FlipOnePressTriggerSwitch()
{
	bOnePressToggle = (bOnePressToggle) ? false : true;
}

/*void ATPShooterCharacter::SetWeaponIndex(bool isUp)
{
	GetRangedWeapon()->LastWeaponIndex = GetRangedWeapon()->WeaponIndex;

	if (IsAbleToSwitchWeapon())
	{
		int32 counter = isUp ? 1 : -1;
		int32 withinRange = (GetRangedWeapon()->WeaponIndex + counter) % WeaponNames.Num();

		GetRangedWeapon()->WeaponIndex = (withinRange >= 0) ? withinRange : WeaponNames.Num() - 1;
		SetWeaponMode(GetRangedWeapon()->WeaponIndex);
		OnSwitchWeapon();
	}
}*/

void ATPShooterCharacter::SetWeaponIndexUp() { GetRangedWeapon()->SetWeaponIndex(true); }
void ATPShooterCharacter::SetWeaponIndexDown() { GetRangedWeapon()->SetWeaponIndex(false); }

/*void ATPShooterCharacter::SetWeaponIndex(const int32 InNumber)
{

	if (InNumber >= WeaponNames.Num()) { return; }

	GetRangedWeapon()->LastWeaponIndex = GetRangedWeapon()->WeaponIndex;

	if ((WeaponNames.Num() > GetRangedWeapon()->WeaponIndex) && IsAbleToSwitchWeapon())
	{
		GetRangedWeapon()->WeaponIndex = InNumber;
		SetWeaponMode(GetRangedWeapon()->WeaponIndex);
		OnSwitchWeapon();
	}
}*/

void ATPShooterCharacter::SetWeaponIndex1() { GetRangedWeapon()->SetWeaponIndex(0); }
void ATPShooterCharacter::SetWeaponIndex2() { GetRangedWeapon()->SetWeaponIndex(1); }
void ATPShooterCharacter::SetWeaponIndex3() { GetRangedWeapon()->SetWeaponIndex(2); }
void ATPShooterCharacter::SetWeaponIndex4() { GetRangedWeapon()->SetWeaponIndex(3); }

// 4.z SWITCH WEAPON

void ATPShooterCharacter::SetWeaponMesh()
{
	USkeletalMeshComponent* WeaponMesh = GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(WeaponMesh);
}

// 5.a PICKUP
void ATPShooterCharacter::AddAmmo(const int32 addAmmo, const EAmmoType ammoType)
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
// 5.z PICKUP

void ATPShooterCharacter::SetIsTransitioningAiming(bool bInBool)
{
	bIsTransitioningAiming = bInBool;
}

bool ATPShooterCharacter::GetTransitioningAiming() const
{
	return bIsTransitioningAiming;
}

float ATPShooterCharacter::AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed)
{
	FVector myVelo = GetVelocity();
	float mySpeed = FVector(myVelo.X, myVelo.Y, 0.0f).Size();
	float divider = (bOtherButtonPressed) ? UKismetMathLibrary::Sqrt(2.0f) : 1.0f;

	return (mySpeed * MyValue) / (divider * GetCharacterMovement()->MaxWalkSpeed);
}