#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPS_FunctionLibrary.h"
#include "TPS_Weapon.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UTPS_Weapon : public UActorComponent
{
	GENERATED_BODY()

public:
	UTPS_Weapon();

protected:

	class ATPS_studyCharacter* TPSCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	class UDataTable* RangedWeaponTable;

	int WeaponIndex;

	TArray<FName> WeaponNames;

	void GetCurrentWeaponMode(int weaponIndex);

	FWeapon CurrentWeapon;
	FProjectile CurrentProjectile;
};
