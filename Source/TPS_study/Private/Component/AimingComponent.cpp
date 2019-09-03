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
	
	SetUpVariables(bShouldDoCheckFile);
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
	bool retVal = (AimingState == EAimingState::Aiming || AimingState == EAimingState::TransitioningAiming && bIsAimingForward == true)
		? true : false;

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
	if (bInBool) AimingState = EAimingState::TransitioningAiming;
}

//===========================================================================
// protected function:
//===========================================================================

void UAimingComponent::BeginPlay()
{
	Super::BeginPlay();

	CameraComponent =  GetComponentSibling<UCameraComponent>();
	CameraBoomComponent = GetComponentSibling<USpringArmComponent>();
	RangedWeaponComponent = GetComponentSibling<URangedWeaponComponent>();

	SetUpVariables(bShouldDoCheckFile);

	// aiming setup:
	if (AimStats.Num() == 0) { AimStats.SetNum(1); };
	AimStats[0].CamBoom.SocketOffset = CameraBoomComponent->SocketOffset;
	AimStats[0].CamBoom.TargetArmLength = CameraBoomComponent->TargetArmLength;
	AimStats[0].CharMov.MaxAcceleration = GetCharacterMovement()->MaxAcceleration;
	AimStats[0].CharMov.MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
	AimStats[0].FollCam.FieldOfView = CameraComponent->FieldOfView;

	AimingNames = AimingTable->GetRowNames();

	int aimingNamesCount = AimingNames.Num();
	FName currentAimingName;
	static const FString contextString(TEXT("Aiming name"));
	struct FAimingStatCompact* aimStatRow;
	AimStats.SetNum(1 + aimingNamesCount);
	UKismetSystemLibrary::PrintString(this, TEXT("added aiming table start loop"), true, false, FLinearColor::Red, 5.0f);
	for (int i = 0; i < AimingNames.Num(); i++)
	{
		currentAimingName = AimingNames[i];
		aimStatRow = AimingTable->FindRow<FAimingStatCompact>(currentAimingName, contextString, true);
		AimStats[1 + i].CamBoom = aimStatRow->AimStat.CamBoom;
		AimStats[1 + i].CharMov = aimStatRow->AimStat.CharMov;
		AimStats[1 + i].FollCam = aimStatRow->AimStat.FollCam;
		UKismetSystemLibrary::PrintString(this, TEXT("added aiming table in aimstat"), true, false, FLinearColor::Red, 5.0f);
	}
}

void UAimingComponent::SetUpVariables(bool bShouldCheck)
{
	if (AimingTable == nullptr)
	{
		//Something* MyClass::aPointer = new Something;
		static ConstructorHelpers::FObjectFinder<UDataTable> tempAimingTable(TEXT("DataTable'/Game/Character/Table/AimingTable.AimingTable'"));
		if (bShouldCheck) check(tempAimingTable.Object);
		AimingTable = tempAimingTable.Object;
	}
	
	if (AimingCurve == nullptr)
	{
		static ConstructorHelpers::FObjectFinder<UCurveFloat> tempAimingCurve(TEXT("CurveFloat'/Game/Character/Curves/AimingFloatCurve.AimingFloatCurve'"));
		if (bShouldCheck) check(tempAimingCurve.Object);
		AimingCurve = tempAimingCurve.Object;
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

	//OnTransitioningAiming.Broadcast(this);
	TimeAiming(AimingAlpha);
	AimingAlpha = AimingCurve->GetFloatValue(CurrentAimingTime / TotalAimingTime);
	//OnAiming.Broadcast(this);
	UE_LOG(LogTemp, Log, TEXT("Aiming Alpha is %f, current time is %f"), AimingAlpha, CurrentAimingTime);

	float incrementTime = (bIsAimingForward) ? DeltaSecond : -1 * DeltaSecond;
	CurrentAimingTime += incrementTime;

	if (bIsAimingForward)
	{
		if (CurrentAimingTime >= TotalAimingTime)
		{
			AimingState = EAimingState::Aiming;

			ClearAndInvalidateAimingTimer(TotalAimingTime);
			//TimeAiming(AimingAlpha);
			OnAiming.Broadcast(this);
			OrientCharacter(true);
			UE_LOG(LogTemp, Log, TEXT("FINISH >>>>>>> current time is %f"), CurrentAimingTime);
		}
	}
	else if (CurrentAimingTime <= 0.0f)
	{
		AimingState = EAimingState::NotAiming;
		OnStopAiming.Broadcast(this);
		//TimeAiming(AimingAlpha);
		OrientCharacter(false);
		ClearAndInvalidateAimingTimer(0.0f);
		UE_LOG(LogTemp, Log, TEXT("<<<<<<<<<<<FINISH  current time is %f"), CurrentAimingTime);
	}
}

void UAimingComponent::ClearAndStartAimingTimer()
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AimingTimerHandle);
	GetOwner()->GetWorldTimerManager().SetTimer(AimingTimerHandle, this, &UAimingComponent::AimingTimerStart, DeltaSecond, true);
	OnTransitioningAiming.Broadcast(this);
}

void UAimingComponent::ClearAndInvalidateAimingTimer(const float NewCurrentTime)
{
	GetOwner()->GetWorldTimerManager().ClearTimer(AimingTimerHandle);
	AimingTimerHandle.Invalidate();
	CurrentAimingTime = NewCurrentTime;
}

void UAimingComponent::TimeAiming(float InAlpha)
{
	UE_LOG(LogTemp, Log, TEXT("aimstats number %i"), AimStats.Num());

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