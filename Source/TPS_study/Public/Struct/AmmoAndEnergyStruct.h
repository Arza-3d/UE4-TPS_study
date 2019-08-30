#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmmoAndEnergyStruct.generated.h"

USTRUCT(BlueprintType)
struct FAmmoCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 StandardAmmo = 15;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 RifleAmmo = 30;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 ShotgunAmmo = 10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 Rocket = 3;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 Arrow = 18;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 Grenade = 2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Meta = (ClampMin = "0", ClampMax = "255"))
	int32 Mine = 3;
};

/**Only external energy mana is not included*/
USTRUCT(BlueprintType)
struct FExternalEnergyCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy", Meta = (ClampMin = "0"))
	float MP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Fuel = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Battery = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Energy")
	float Overheat = 0.0f;
};

UCLASS()
class TPS_STUDY_API UAmmoAndEnergyStruct : public UObject
{
	GENERATED_BODY()
	
};
