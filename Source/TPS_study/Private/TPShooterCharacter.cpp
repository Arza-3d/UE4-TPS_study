#include "TPShooterCharacter.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CustomCollisionChannel.h"
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

//==============
// Construction:
//==============

ATPShooterCharacter::ATPShooterCharacter()
{
	// basic component setup:
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	RangedWeapon = CreateDefaultSubobject<URangedWeaponComponent>(TEXT("RangedWeapon"));
	HealthAndMana = CreateDefaultSubobject<UHPandMPComponent>(TEXT("HealthAndMana"));

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

	PrimaryActorTick.bStartWithTickEnabled = true;
}

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



//===========================================================================
// protected function:
//===========================================================================

void ATPShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATPShooterCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	GetRangedWeapon()->DeltaSecond = DeltaSeconds;
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

UAnimInstance* ATPShooterCharacter::GetAnimBlueprint() const
{
	return GetMesh()->GetAnimInstance();
}

//========================
// Navigation (protected):
//========================

float ATPShooterCharacter::GetNormalizedForward() const
{
	return NormalizedForward;
}

float ATPShooterCharacter::GetNormalizedRight() const
{
	return NormalizedRight;
}

//================================
// Overriden function (protected):
//================================

bool ATPShooterCharacter::IsAbleToSwitchWeapon_Implementation()
{
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetRangedWeapon()->GetIsAiming();

	return bIsOnTheGround && bIsNotAiming;
}

bool ATPShooterCharacter::IsAbleToAim_Implementation()
{
	return !GetCharacterMovement()->IsFalling();
}

bool ATPShooterCharacter::IsAbleToFire_Implementation()
{
	return !GetCharacterMovement()->IsFalling();
}

//===========================================================================
// private function:
//===========================================================================

//==================
// Aiming (private):
//==================

void ATPShooterCharacter::AimingPress() 
{
	GetRangedWeapon()->AimingPress();
}

void ATPShooterCharacter::AimingRelease()
{
	GetRangedWeapon()->AimingRelease();
}

//=======================
// Fire Weapon (private):
//=======================

void ATPShooterCharacter::FirePress()
{
	GetRangedWeapon()->FirePress();
}

void ATPShooterCharacter::FireRelease()
{
	GetRangedWeapon()->FireRelease();
}

//=========================
// Switch Weapon (private):
//=========================

void ATPShooterCharacter::SetWeaponIndexUp() { GetRangedWeapon()->SetWeaponIndex(true); }
void ATPShooterCharacter::SetWeaponIndexDown() { GetRangedWeapon()->SetWeaponIndex(false); }

void ATPShooterCharacter::SetWeaponIndex1() { GetRangedWeapon()->SetWeaponIndex(0); }
void ATPShooterCharacter::SetWeaponIndex2() { GetRangedWeapon()->SetWeaponIndex(1); }
void ATPShooterCharacter::SetWeaponIndex3() { GetRangedWeapon()->SetWeaponIndex(2); }
void ATPShooterCharacter::SetWeaponIndex4() { GetRangedWeapon()->SetWeaponIndex(3); }

//======================
// Navigation (private):
//======================

float ATPShooterCharacter::AssignNormalizedVelo(float MyValue, bool bOtherButtonPressed)
{
	FVector myVelo = GetVelocity();
	float mySpeed = FVector(myVelo.X, myVelo.Y, 0.0f).Size();
	float divider = (bOtherButtonPressed) ? UKismetMathLibrary::Sqrt(2.0f) : 1.0f;

	return (mySpeed * MyValue) / (divider * GetCharacterMovement()->MaxWalkSpeed);
}

void ATPShooterCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);

		if (GetRangedWeapon()->AimingState != EAimingState::NotAiming)
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

		if (GetRangedWeapon()->AimingState != EAimingState::NotAiming)
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