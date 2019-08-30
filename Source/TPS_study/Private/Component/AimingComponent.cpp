#include "Component/AimingComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "TimerManager.h"

#include "Component/RangedWeaponComponent.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

//===========================================================================
// public function:
//===========================================================================

UAimingComponent::UAimingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAimingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	DeltaSecond = DeltaTime;
}

//=================
// Getter (public):
//=================

bool UAimingComponent::GetIsAiming() const
{
	bool retVal = AimingState == EAimingState::Aiming;

	if (RangedWeaponComponent == nullptr) return retVal;

	return (RangedWeaponComponent->bIsAbleToShootWithoutAiming) ? true : retVal;
}

bool UAimingComponent::GetTransitioningAiming() const
{
	return AimingState == EAimingState::TransitioningAiming;
}

void UAimingComponent::AimingPress()
{
	bIsAimingForward = true;
	ClearAndStartAimingTimer();
	UE_LOG(LogTemp, Log, TEXT("Start Timer"));
}

void UAimingComponent::AimingRelease()
{
	bIsAimingForward = false;
	ClearAndStartAimingTimer();
	UE_LOG(LogTemp, Log, TEXT("STOP Timer DELTA seconds is %f"), DeltaSecond);
}

float UAimingComponent::GetAimingAlpha() const
{
	return AimingAlpha;
}

//=================
// Setter (public):
//=================

void UAimingComponent::SetIsTransitioningAiming(bool bInBool)
{
	if (bInBool)
	{
		AimingState = EAimingState::TransitioningAiming;
	}
}

//===========================================================================
// protected function:
//===========================================================================

void UAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent = GetThisType<UCameraComponent>();
	CameraBoomComponent = GetThisType<USpringArmComponent>();
	RangedWeaponComponent = GetThisType<URangedWeaponComponent>();

	if (AimingCurve == nullptr)
	{
		//GEngine->ClearOnScreenDebugMessages();
		UKismetSystemLibrary::PrintString(this, FString("AIMING CURVE IS EMPTY!!! >O<"), true, false, FLinearColor::Red, 10.0f);
	}

	if (AimingTable != nullptr)
	{
		AimingNames = AimingTable->GetRowNames();
	}

	// aiming setup:
	if (AimStats.Num() == 0) { AimStats.SetNum(1); };
	AimStats[0].CamBoom.SocketOffset = CameraBoomComponent->SocketOffset;
	AimStats[0].CamBoom.TargetArmLength = CameraBoomComponent->TargetArmLength;
	AimStats[0].CharMov.MaxAcceleration = GetCharacterMovement()->MaxAcceleration;
	AimStats[0].CharMov.MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	AimStats[0].FollCam.FieldOfView = CameraComponent->FieldOfView;

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
}

//==================
// Aiming (private):
//==================

ACharacter* UAimingComponent::GetCharacter()
{
	return Cast<ACharacter>(GetOwner());
}

UCharacterMovementComponent* UAimingComponent::GetCharacterMovement()
{
	return Cast<ACharacter>(GetOwner())->GetCharacterMovement();
}

void UAimingComponent::AimingTimerStart()
{
	AimingState = EAimingState::TransitioningAiming;

	OnTransitioningAiming.Broadcast(this);

	AimingAlpha = AimingCurve->GetFloatValue(CurrentAimingTime / TotalAimingTime);

	UE_LOG(LogTemp, Log, TEXT("Aiming Alpha is %f, current time is %f"), AimingAlpha, CurrentAimingTime);

	float incrementTime = (bIsAimingForward) ? DeltaSecond : -1 * DeltaSecond;
	CurrentAimingTime += incrementTime;

	if (bIsAimingForward)
	{
		if (CurrentAimingTime >= TotalAimingTime)
		{
			AimingState = EAimingState::Aiming;

			ClearAndInvalidateAimingTimer(TotalAimingTime);

			OnAiming.Broadcast(this);
			UE_LOG(LogTemp, Log, TEXT("FINISH >>>>>>> current time is %f"), CurrentAimingTime);
		}
	}
	else if (CurrentAimingTime <= 0.0f)
	{
		AimingState = EAimingState::NotAiming;

		OnStopAiming.Broadcast(this);

		ClearAndInvalidateAimingTimer(0.0f);
		UE_LOG(LogTemp, Log, TEXT("<<<<<<<<<<<FINISH  current time is %f"), CurrentAimingTime);
	}
}

void UAimingComponent::ClearAndStartAimingTimer()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AimingTimerHandle);
	GetOwner()->GetWorldTimerManager().SetTimer(AimingTimerHandle, this, &UAimingComponent::AimingTimerStart, DeltaSecond, true);
}

void UAimingComponent::ClearAndInvalidateAimingTimer(const float NewCurrentTime)
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AimingTimerHandle);
	AimingTimerHandle.Invalidate();
	CurrentAimingTime = NewCurrentTime;
}

void UAimingComponent::TimeAiming(float InAlpha)
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

	CameraBoomComponent->TargetArmLength = FMath::Lerp(defaultTargetArmLength, aimingTargetArmLength, InAlpha);
	CameraBoomComponent->SocketOffset = FMath::Lerp(defaultSocketOffset, aimingSocketOffset, InAlpha);
	GetCharacter()->GetCharacterMovement()->MaxWalkSpeed = FMath::Lerp(defaultWalkSpeed, aimingWalkSpeed, InAlpha);
	GetCharacter()->GetCharacterMovement()->MaxAcceleration = FMath::Lerp(defaultMaxAcceleration, aimingMaxAcceleration, InAlpha);
	CameraComponent->SetFieldOfView(FMath::Lerp(defaultFieldOfView, aimingFieldOfView, InAlpha));
}

void UAimingComponent::OrientCharacter(bool bMyCharIsAiming)
{
	CameraComponent->bUsePawnControlRotation = bMyCharIsAiming;
	GetCharacter()->bUseControllerRotationYaw = bMyCharIsAiming;
	GetCharacter()->GetCharacterMovement()->bOrientRotationToMovement = !bMyCharIsAiming;
}