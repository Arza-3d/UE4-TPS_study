#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AmmoAndEnergyStruct.generated.h"

USTRUCT(BlueprintType)
struct FAmmoCount
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int StandardAmmo = 15;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int RifleAmmo = 30;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int ShotgunAmmo = 10;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int Rocket = 3;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int Arrow = 18;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int Grenade = 2;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo", Meta = (ClampMin = "0", ClampMax = "255"))
		int Mine = 3;
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
