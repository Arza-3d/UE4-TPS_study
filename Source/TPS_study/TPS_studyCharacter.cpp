#include "TPS_studyCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/DataTable.h"
//#include "Components/TimelineComponent.h"

ATPS_studyCharacter::ATPS_studyCharacter()
{
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

	PlayerInputComponent->BindAxis("MoveForward", this, &ATPS_studyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATPS_studyCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATPS_studyCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATPS_studyCharacter::LookUpAtRate);
}

void ATPS_studyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (WeaponTable != nullptr)
	{
		WeaponNames = WeaponTable->GetRowNames();
	}
}

void ATPS_studyCharacter::SetIsTransitioningAiming(bool isTransitioningAiming)
{
	bIsTransitioningAiming = isTransitioningAiming;
}

bool ATPS_studyCharacter::GetTransitioningAiming()
{
	return bIsTransitioningAiming;
}

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

void ATPS_studyCharacter::Aiming(bool bIsCharAiming)
{
	bIsAiming = bIsCharAiming;
	OrientCharacter(bIsCharAiming);
}

void ATPS_studyCharacter::SetWeaponIndexWithNumpad(int numberInput)
{
	LastWeaponIndex = WeaponIndex;

	if (IsWeaponNameInThisIndexExist(numberInput) && IsSwitchWeaponRequirementFulfilled())
	{
		WeaponIndex = numberInput;
	}
	else
	{
		return;
	}
}

void ATPS_studyCharacter::SetWeaponIndexWithMouseWheel(bool isUp)
{
	LastWeaponIndex = WeaponIndex;
	
	if (IsSwitchWeaponRequirementFulfilled())
	{
		int counter = isUp ? 1 : -1;
		int withinRange = (WeaponIndex + counter) % WeaponNames.Num();
		if(withinRange >= 0)
		{
			WeaponIndex = withinRange;
		}
		else
		{
			WeaponIndex = WeaponNames.Num() - 1;
		}
	}
	else
	{
		return;
	}
}

bool ATPS_studyCharacter::IsWeaponNameInThisIndexExist(int weaponIndex)
{
	return WeaponNames.Num() > weaponIndex;
}

bool ATPS_studyCharacter::IsAbleToRepeatAutoFire_Implementation()
{
	return bIsTriggerPressed;
}

bool ATPS_studyCharacter::IsSwitchWeaponRequirementFulfilled_Implementation()
{
	bool bIsOnTheGround = !GetCharacterMovement()->IsFalling();
	bool bIsNotAiming = !GetIsAiming();

	return bIsOnTheGround && bIsNotAiming;
}

void ATPS_studyCharacter::OrientCharacter(bool bMyCharIsAiming)
{
	FollowCamera->bUsePawnControlRotation = bMyCharIsAiming;
	bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}

void ATPS_studyCharacter::GetCurrentWeaponMode(int weaponIndex)
{
	FName currentWeaponName = WeaponNames[weaponIndex];
	static const FString contextString(TEXT("Weapon Mode"));
	struct FWeaponMode* currentWeaponMode;

	currentWeaponMode = WeaponTable->FindRow<FWeaponMode>(currentWeaponName, contextString, true);
	ShooterState = currentWeaponMode->Shooter;
	CurrentWeapon = currentWeaponMode->Weapon;
	CurrentProjectile = currentWeaponMode->Projectile;
}

void ATPS_studyCharacter::MainFire(bool isTriggerPressed)
{
	bIsTriggerPressed = isTriggerPressed;


}

bool ATPS_studyCharacter::GetIsAiming()
{
	return bIsAiming;
}

bool ATPS_studyCharacter::GetIsTriggerPressed()
{
	return bIsTriggerPressed;
}

float ATPS_studyCharacter::GetNewPlayRate(UAnimMontage* animMontage, float fireRate)
{
	if (fireRate > 0.0f)
	{
		return 1.0f;
	}
	else
	{
		return animMontage->SequenceLength / fireRate;
	}
}

float ATPS_studyCharacter::GetNormalizedForward()
{
	return NormalizedForward;
}

float ATPS_studyCharacter::GetNormalizedRight()
{
	return NormalizedRight;
}
