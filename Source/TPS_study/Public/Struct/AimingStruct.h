#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "AimingStruct.generated.h"

//===============
// Aiming Struct:
//===============

USTRUCT(BlueprintType)
struct FCharMovAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxAcceleration = 2048.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float MaxWalkSpeed = 600.0f;
};

USTRUCT(BlueprintType)
struct FCamBoomAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FVector SocketOffset;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float TargetArmLength = 300.0f;
};

USTRUCT(BlueprintType)
struct FollowCamAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float FieldOfView = 90.0f;
};

USTRUCT(BlueprintType)
struct FAimingStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FCharMovAimingStat CharMov;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FCamBoomAimingStat CamBoom;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FollowCamAimingStat FollCam;
};

USTRUCT(BlueprintType)
struct FAimingStatCompact : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FAimingStat AimStat;
};

UCLASS()
class TPS_STUDY_API UAimStruct : public UObject
{
	GENERATED_BODY()
	
};
