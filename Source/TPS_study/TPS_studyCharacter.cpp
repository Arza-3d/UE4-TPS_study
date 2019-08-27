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
#include "HPandMPComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "RangedWeaponComponent.h"
#include "TimerManager.h"
#include "TPSAnimInterface.h"
#include "TPSFunctionLibrary.h"
#include "TPS_Projectile.h"
#include "DrawDebugHelpers.h"

// 0.a CONSTRUCTION
ATPS_studyCharacter::ATPS_studyCharacter()
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

float ATPS_studyCharacter::GetHP() const
{ 
	return CharacterStat.HP; 
}

void ATPS_studyCharacter::SetHP(float val)
{ 
	CharacterStat.HP = val; 
}

float ATPS_studyCharacter::GetMP() const 
{
	return CharacterStat.MP;
}

void ATPS_studyCharacter::SetMP(float val) 
{
	CharacterStat.MP = val; 
}

void ATPS_studyCharacter::BeginPlay() {
	Super::BeginPlay();

	// aiming setup:
	if (AimingTable != nullptr)
	{ 
		AimingNames = AimingTable->GetRowNames(); 
	}

	int aimingNamesCount = AimingNames.Num();
	FName currentAimingName;
	static const FString contextString(TEXT("Aiming name"));
	struct FAimingStatCompact* aimStatRow;
	AimStats.SetNum(1 + aimingNamesCount);

	for (int i = 0; i < AimingNames.Num(); i++) 
	{
		currentAimingName = AimingNames[i];
		aimStatRow = AimingTable->FindRow<FAimingStatCompact>(currentAimingName, contextString, true);
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
	if (RangedWeapon->WeaponTable != nullptr)
	{ 
		WeaponNames = RangedWeapon->WeaponTable->GetRowNames();
	}

	SetWeaponMode(0);
}

void ATPS_studyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("AttackAction", IE_Pressed, this, &ATPS_studyCharacter::FirePress);
	PlayerInputComponent->BindAction("AttackAction", IE_Released, this, &ATPS_studyCharacter::FireRelease);
	PlayerInputComponent->BindAction("AimAction", IE_Pressed, this, &ATPS_studyCharacter::AimingPress);
	PlayerInputComponent->BindAction("AimAction", IE_Released, this, &ATPS_studyCharacter::AimingRelease);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Weapon1", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndex1);
	PlayerInputComponent->BindAction("Weapon2", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndex2);
	PlayerInputComponent->BindAction("Weapon3", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndex3);
	PlayerInputComponent->BindAction("Weapon4", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndex4);
	PlayerInputComponent->BindAction("ChangeWeaponUp", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexUp);
	PlayerInputComponent->BindAction("ChangeWeaponDown", IE_Pressed, this, &ATPS_studyCharacter::SetWeaponIndexDown);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPS_studyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPS_studyCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPS_studyCharacter::LookRightAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPS_studyCharacter::LookUpAtRate);
}

void ATPS_studyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AimingTimeline) 
	{
		AimingTimeline->TickComponent(DeltaSeconds, ELevelTick::LEVELTICK_TimeOnly, NULL);
	}
}
// 0.z CONSTRUCTION

// 1.a NAVIGATION
float ATPS_studyCharacter::GetNormalizedForward() const
{
	return NormalizedForward; 
}

float ATPS_studyCharacter::GetNormalizedRight() const 
{
	return NormalizedRight;
}

ETriggerMechanism ATPS_studyCharacter::GetTriggerMechanism() const 
{
	return CurrentWeapon.Trigger;
}

FName ATPS_studyCharacter::GetWeaponName() const 
{
	return WeaponNames[WeaponIndex];
}

void ATPS_studyCharacter::MoveForward(float Value)
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

void ATPS_studyCharacter::MoveRight(float Value)
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

void ATPS_studyCharacter::LookRightAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATPS_studyCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
// 1.z NAVIGATION

// 2.a AIMING
bool ATPS_studyCharacter::GetIsAiming() const
{
	return (bWeaponIsAlwaysAiming) ? true : bIsAiming;
}

void ATPS_studyCharacter::AimingPress() {

	if (!IsAbleToAim()) { return; }

	Aiming(true);

	OnAiming();

	bIsTransitioningAiming = true;

	AimingTimeline->Play();
}

void ATPS_studyCharacter::AimingRelease()
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

void ATPS_studyCharacter::Aiming(const bool bInIsAiming)
{
	bIsAiming = bInIsAiming;
	OrientCharacter(bInIsAiming);
	bIsTransitioningAiming = bInIsAiming;

	float speed = (bInIsAiming) ? RangedWeapon->AimingSpeed : RangedWeapon->StopAimingSpeed;

	if (speed <= 0.0f) { speed = 1.0f; };

	AimingTimeline->SetPlayRate(1.0f / speed);
}

void ATPS_studyCharacter::OrientCharacter(bool bMyCharIsAiming)
{
	FollowCamera->bUsePawnControlRotation = bMyCharIsAiming;
	bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}

void ATPS_studyCharacter::TimeAiming(float Alpha)
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
	
	GetCameraBoom()->TargetArmLength = FMath::Lerp(defaultTargetArmLength, aimingTargetArmLength, Alpha);
	GetCameraBoom()->SocketOffset = FMath::Lerp(defaultSocketOffset, aimingSocketOffset, Alpha);
	GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(defaultWalkSpeed, aimingWalkSpeed, Alpha);
	GetCharacterMovement()->MaxAcceleration = FMath::Lerp(defaultMaxAcceleration, aimingMaxAcceleration, Alpha);
	GetFollowCamera()->SetFieldOfView(FMath::Lerp(defaultFieldOfView, aimingFieldOfView, Alpha));
}

void ATPS_studyCharacter::TimeFinishAiming() 
{ 
	bIsTransitioningAiming = false; 
	OnAiming();
}
// 2.z AIMING

// 3.a FIRE
bool ATPS_studyCharacter::IsAbleToRepeatAutoFire_Implementation() 
{
	return bIsTriggerPressed; 
}

bool ATPS_studyCharacter::IsAbleToFire_Implementation()
{
	return !GetCharacterMovement()->IsFalling(); 
}

bool ATPS_studyCharacter::IsAmmoEnough() 
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

bool ATPS_studyCharacter::GetIsTriggerPressed() const
{
	return bIsTriggerPressed;
}

FRotator ATPS_studyCharacter::GetNewMuzzleRotationFromLineTrace(FTransform SocketTransform) 
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

void ATPS_studyCharacter::FirePress() 
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

void ATPS_studyCharacter::FireRelease()
{
	bIsTriggerPressed = false;

	if (CurrentWeapon.Trigger == ETriggerMechanism::ReleaseTrigger) 
	{
		FireReleaseAfterHold();
	}
}

void ATPS_studyCharacter::FireAutomaticTrigger()
{
	if (!(bIsTriggerPressed && IsWeaponAbleToFire()))
	{
		return; 
	}

	FireStandardTrigger();
}

bool ATPS_studyCharacter::IsWeaponAbleToFire()
{
	return GetIsAiming() && bIsFireRatePassed && IsAmmoEnough();
}

void ATPS_studyCharacter::FireAutomaticTriggerOnePress()
{
	if (bOnePressToggle && IsWeaponAbleToFire()) 
	{
		FireStandardTrigger();
	}
}

void ATPS_studyCharacter::FireHold() 
{
	GetWorldTimerManager().ClearTimer(TimerOfHoldTrigger);
	GetWorldTimerManager().SetTimer(TimerOfHoldTrigger, this, &ATPS_studyCharacter::CountHoldTriggerTime, HoldTimeRateCount, true);
}

void ATPS_studyCharacter::CountHoldTriggerTime()
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

void ATPS_studyCharacter::FireReleaseAfterHold() 
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

void ATPS_studyCharacter::FireStandardTrigger()
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

void ATPS_studyCharacter::FireProjectile(const EAmmoType AmmoType)
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

void ATPS_studyCharacter::FireProjectile(const EEnergyType EnergyType)
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

void ATPS_studyCharacter::FireProjectile() 
{
	TArray<FName> MuzzleName = CurrentWeapon.SocketName;
	int MuzzleCount = MuzzleName.Num();

	for (int i = 0; i < MuzzleCount; i++) 
	{
		SpawnProjectile(WeaponInWorld, MuzzleName, GetWorld(), i);
	}
}

void ATPS_studyCharacter::FireProjectile(int* Ammo)
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

void ATPS_studyCharacter::FireProjectile(float* MyEnergy) 
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

void ATPS_studyCharacter::SpawnProjectile(USceneComponent* MyWeaponInWorld, TArray<FName> MuzzleName, UWorld* MyWorld, int32 i)
{
	FTransform MuzzleTransform = MyWeaponInWorld->GetSocketTransform(MuzzleName[i]);
	FTransform SpawnTransform = FTransform(GetNewMuzzleRotationFromLineTrace(MuzzleTransform), MuzzleTransform.GetLocation(), MuzzleTransform.GetScale3D());

	ATPS_Projectile* MyProjectile = MyWorld->SpawnActorDeferred<ATPS_Projectile>(ATPS_Projectile::StaticClass(), SpawnTransform);

	MyProjectile->SetUpProjectile(CurrentProjectile);
	MyProjectile->FinishSpawning(SpawnTransform);
}

void ATPS_studyCharacter::PlayFireMontage() 
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

void ATPS_studyCharacter::TimerFireRateStart() 
{
	bIsFireRatePassed = false;

	GetWorldTimerManager().ClearTimer(FireRateTimer);
	GetWorldTimerManager().SetTimer(FireRateTimer, this, &ATPS_studyCharacter::TimerFireRateReset, CurrentWeapon.FireRateAndOther[0]);
}

void ATPS_studyCharacter::TimerFireRateReset()
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

bool ATPS_studyCharacter::IsAmmoEnough(const int32 InAmmo)
{
	bool bAmmoIsEmpty = InAmmo <= 0;

	if (bAmmoIsEmpty) 
	{ 
		OnNoAmmo(); 
	}

	return !bAmmoIsEmpty;
}

bool ATPS_studyCharacter::IsAmmoEnough(const EAmmoType Ammo)
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

bool ATPS_studyCharacter::IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot)
{
	bool bIsNotEnoughEnergy = MyEnergy < MyEnergyPerShot;

	if (bIsNotEnoughEnergy) 
	{
		OnNoEnergy(); 
	}

	return !bIsNotEnoughEnergy;
}

bool ATPS_studyCharacter::IsAmmoEnough(const EEnergyType EnergyType)
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

bool ATPS_studyCharacter::IsWeaponNotOverheating()
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
bool ATPS_studyCharacter::IsAbleToSwitchWeapon_Implementation() 
{
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetIsAiming();

	return bIsOnTheGround && bIsNotAiming;
}

bool ATPS_studyCharacter::IsAbleToAim_Implementation() 
{
	return !GetCharacterMovement()->IsFalling();
}

int32 ATPS_studyCharacter::GetLastWeaponIndex() const 
{
	return LastWeaponIndex; 
}

int32 ATPS_studyCharacter::GetWeaponIndex() const
{
	return WeaponIndex; 
}

void ATPS_studyCharacter::SetWeaponMode(const int32 MyWeaponIndex) 
{
	FName CurrentWeaponName = WeaponNames[MyWeaponIndex];
	static const FString ContextString(TEXT("Weapon Mode"));
	struct FWeaponModeCompact* WeaponModeRow;
	WeaponModeRow = RangedWeapon->WeaponTable->FindRow<FWeaponModeCompact>(CurrentWeaponName, ContextString, true);

	if (WeaponModeRow) 
	{
		FWeaponMode CurrentWeaponMode = WeaponModeRow->WeaponMode;

		ShooterState = CurrentWeaponMode.Shooter;
		CurrentWeapon = CurrentWeaponMode.Weapon;
		CurrentProjectile = CurrentWeaponMode.Projectile;

		if (CurrentWeapon.FireRateAndOther.Num() >= 2 )
		{
			MaxFireHoldTime = CurrentWeapon.FireRateAndOther[1];
		}
		else
		{
			MaxFireHoldTime = CurrentWeapon.FireRateAndOther[0];
		}
	}
}

void ATPS_studyCharacter::SetWeaponIndex(bool isUp) 
{
	LastWeaponIndex = WeaponIndex;

	if (IsAbleToSwitchWeapon()) 
	{
		int32 counter = isUp ? 1 : -1;
		int32 withinRange = (WeaponIndex + counter) % WeaponNames.Num();

		WeaponIndex = (withinRange >= 0) ? withinRange : WeaponNames.Num() - 1;
		SetWeaponMode(WeaponIndex);
		OnSwitchWeapon();
	}
}

void ATPS_studyCharacter::SetWeaponIndexUp() { SetWeaponIndex(true); }
void ATPS_studyCharacter::SetWeaponIndexDown() { SetWeaponIndex(false); }

void ATPS_studyCharacter::FlipOnePressTriggerSwitch()
{
	bOnePressToggle = (bOnePressToggle) ? false : true;
}

void ATPS_studyCharacter::SetWeaponIndex(const int32 InNumber)
{

	if (InNumber >= WeaponNames.Num()) { return; }

	LastWeaponIndex = WeaponIndex;

	if ((WeaponNames.Num() > WeaponIndex) && IsAbleToSwitchWeapon()) 
	{
		WeaponIndex = InNumber;
		SetWeaponMode(WeaponIndex);
		OnSwitchWeapon();
	}
}

void ATPS_studyCharacter::SetWeaponIndex1() { SetWeaponIndex(0); }
void ATPS_studyCharacter::SetWeaponIndex2() { SetWeaponIndex(1); }
void ATPS_studyCharacter::SetWeaponIndex3() { SetWeaponIndex(2); }
void ATPS_studyCharacter::SetWeaponIndex4() { SetWeaponIndex(3); }

// 4.z SWITCH WEAPON

void ATPS_studyCharacter::SetWeaponMesh()
{
	USkeletalMeshComponent* WeaponMesh = GetMesh(); // change it to accept additional weapon mesh later
	WeaponInWorld = Cast<USceneComponent>(WeaponMesh);
}

// 5.a PICKUP
void ATPS_studyCharacter::AddAmmo(const int32 addAmmo, const EAmmoType ammoType) 
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

void ATPS_studyCharacter::SetIsTransitioningAiming(bool bInBool) 
{
	bIsTransitioningAiming = bInBool;
}

bool ATPS_studyCharacter::GetTransitioningAiming() const
{
	return bIsTransitioningAiming;
}

float ATPS_studyCharacter::AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed) 
{
	FVector myVelo = GetVelocity();
	float mySpeed = FVector(myVelo.X, myVelo.Y, 0.0f).Size();
	float divider = (bOtherButtonPressed) ? UKismetMathLibrary::Sqrt(2.0f) : 1.0f;

	return (mySpeed * MyValue) / (divider * GetCharacterMovement()->MaxWalkSpeed);
}