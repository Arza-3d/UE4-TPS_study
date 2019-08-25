#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TPSFunctionLibrary.h"
#include "RangedWeaponComponent.generated.h"

class UDataTable;
class UUserWidget;
class ATPShooterCharacter;
class ATPS_studyCharacter;

//=============================================================================
/**
 * RangedWeaponComponent handles projectile based attack logic
 * It contains:
 *  * Weapon table
 *  * Aiming table
 *  * Aiming functionality
 *  * Fire functionality
 *  * Switch weapon functionality
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API URangedWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	friend ATPS_studyCharacter;
	friend ATPShooterCharacter;

//===========================================================================
public:	
//===========================================================================

	URangedWeaponComponent();

	//=================
	// Getter (public):
	//=================

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int32 GetWeaponIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	int32 GetLastWeaponIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
	FName GetWeaponName() const;

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float AimingSpeed = 0.1f;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Aiming")
	float StopAimingSpeed = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Aiming")
	TSubclassOf<UUserWidget> Crosshair;

	UPROPERTY(EditDefaultsOnly, Category = "Aiming")
	UDataTable* AimingTable;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UDataTable* WeaponTable;
	
//===========================================================================
private:
//===========================================================================

	ATPShooterCharacter* Shooter;
	USceneComponent* WeaponInWorld;

	void SetWeaponMesh();

	int32 WeaponIndex;
	int32 LastWeaponIndex;
	TArray<FName> WeaponNames;

	void SetWeaponMode(const int32 MyWeaponIndex);
	void SetWeaponIndex(const int32 InNumber);
	void SetWeaponIndex(const bool isUp);

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType InAmmoType);
	bool IsAmmoEnough(const EEnergyType InEnergyType);
	bool IsAmmoEnough(const int32 InAmmo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();
};
