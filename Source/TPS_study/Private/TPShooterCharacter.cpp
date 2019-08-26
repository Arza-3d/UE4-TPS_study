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
	//GetRangedWeapon()->SetWeaponMesh();
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

//================
// 1.a NAVIGATION:
//================

float ATPShooterCharacter::GetNormalizedForward() const
{
	return NormalizedForward;
}

float ATPShooterCharacter::GetNormalizedRight() const
{
	return NormalizedRight;
}

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
	return (bIsAbleToShootWithoutAiming) ? true : bIsAiming;
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

	GetRangedWeapon()->bOnePressToggle = false;

	if (GetRangedWeapon()->CurrentWeapon.Trigger == ETriggerMechanism::ReleaseTrigger)
	{
		GetWorldTimerManager().ClearTimer(GetRangedWeapon()->TimerOfHoldTrigger);
		GetRangedWeapon()->bMaxHoldIsReach = false;
		GetRangedWeapon()->HoldTime = 0.0f;
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

// 3.a FIRE
bool ATPShooterCharacter::IsAbleToRepeatAutoFire_Implementation()
{
	return GetRangedWeapon()->bIsTriggerPressed;
}

bool ATPShooterCharacter::IsAbleToFire_Implementation()
{
	return !GetCharacterMovement()->IsFalling();
}

void ATPShooterCharacter::FirePress()
{
	GetRangedWeapon()->FirePress();
}

void ATPShooterCharacter::FireRelease()
{
	GetRangedWeapon()->FireRelease();
}

// 3.z FIRE

//===================
// 4.a SWITCH WEAPON:
//===================

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

void ATPShooterCharacter::SetWeaponIndexUp() { GetRangedWeapon()->SetWeaponIndex(true); }
void ATPShooterCharacter::SetWeaponIndexDown() { GetRangedWeapon()->SetWeaponIndex(false); }

void ATPShooterCharacter::SetWeaponIndex1() { GetRangedWeapon()->SetWeaponIndex(0); }
void ATPShooterCharacter::SetWeaponIndex2() { GetRangedWeapon()->SetWeaponIndex(1); }
void ATPShooterCharacter::SetWeaponIndex3() { GetRangedWeapon()->SetWeaponIndex(2); }
void ATPShooterCharacter::SetWeaponIndex4() { GetRangedWeapon()->SetWeaponIndex(3); }

// 5.a PICKUP
/*void ATPShooterCharacter::AddAmmo(const int32 addAmmo, const EAmmoType ammoType)
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
}*/

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