#include "Character/TPShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

#include "Component/RangedWeaponComponent.h"
#include "Component/AmmoAndEnergyComponent.h"
#include "Component/HPandMPComponent.h"
#include "Component/AimingComponent.h"
#include "Interface/TPSAnimInterface.h"

#include "UObject/ConstructorHelpers.h"
#include "Engine/DataTable.h"

//==============
// Construction:
//==============

ATPShooterCharacter::ATPShooterCharacter()
{
	// basic component setup:
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));

	HealthAndMana = CreateDefaultSubobject<UHPandMPComponent>(TEXT("HealthAndMana"));
	RangedWeapon = CreateDefaultSubobject<URangedWeaponComponent>(TEXT("RangedWeapon"));
	Aiming = CreateDefaultSubobject<UAimingComponent>(TEXT("Aiming"));
	Ammo = CreateDefaultSubobject<UAmmoAndEnergyComponent>(TEXT("Ammo"));

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

//===========================================================================
// protected function:
//===========================================================================

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
	bool bIsNotAiming = !Aiming->GetIsAiming();

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
	Aiming->AimingPress();
}

void ATPShooterCharacter::AimingRelease()
{
	Aiming->AimingRelease();
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

void ATPShooterCharacter::SetWeaponIndexUp() { GetRangedWeapon()->SetWeaponIndexWithMouseWheel(true); }
void ATPShooterCharacter::SetWeaponIndexDown() { GetRangedWeapon()->SetWeaponIndexWithMouseWheel(false); }

void ATPShooterCharacter::SetWeaponIndex1() { GetRangedWeapon()->SetWeaponIndexWithNumpad(0); }
void ATPShooterCharacter::SetWeaponIndex2() { GetRangedWeapon()->SetWeaponIndexWithNumpad(1); }
void ATPShooterCharacter::SetWeaponIndex3() { GetRangedWeapon()->SetWeaponIndexWithNumpad(2); }
void ATPShooterCharacter::SetWeaponIndex4() { GetRangedWeapon()->SetWeaponIndexWithNumpad(3); }

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

		if (Aiming->GetIsAiming())
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

		if (Aiming->GetIsAiming())
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
