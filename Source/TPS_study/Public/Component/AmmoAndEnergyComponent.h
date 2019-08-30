#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Struct/TableStruct/WeaponTableStruct.h"
#include "Struct/AmmoAndEnergyStruct.h"

#include "AmmoAndEnergyComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRunOutOfAmmoSignature, UAmmoAndEnergyComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIsOverheatingSignature, UAmmoAndEnergyComponent*, MyComponent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNoMoreAmmoDuringFire, const int32, MyFireRound);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRunOutOfEnergySignature, UAmmoAndEnergyComponent*, MyComponent, const float, CurrentEnergy, const float, EnergyNeededPerShot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UAmmoAndEnergyComponent : public UActorComponent
{
	GENERATED_BODY()

	friend class URangedWeaponComponent;

public:	
	
	UAmmoAndEnergyComponent();

	/** Called when actor can't shoot more during multiple shot  */
	UPROPERTY(BlueprintAssignable, Category = "Fire Event")
	FOnNoMoreAmmoDuringFire OnNoMoreAmmoDuringMultipleShot;

	/** Called when actor can't shoot due to no more ammo */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnRunOutOfAmmoSignature OnAmmoOut;

	/** Called when actor can't shoot due to not enough energy */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnRunOutOfEnergySignature OnEnergyOut;

	/** Called when actor can't shoot due to weapon overheating */
	UPROPERTY(BlueprintAssignable, Category = "Can't Shoot Event")
	FOnIsOverheatingSignature OnOverhating;

	//=================
	// Setter (public):
	//=================

	UFUNCTION(BlueprintCallable, Category = "Ammo")
	void AddAmmo(const EAmmoType InAmmoType, const int32 AdditionalAmmo);

	UFUNCTION(BlueprintCallable, Category = "Energy")
	void AddEnergy(const EEnergyType InEnergyType, const float AdditionalEnergy);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ammo")
	FAmmoCount GetAllAmmo() const;

	/**
	 * if member is 0 will use default value from Struct
	 *    and will have no max ammunition value
	 * -------------------------------------------------
	 * if member is not 0, then (optional):
	 *  * 0 = initial ammunition
	 *  * 1 = max ammunition
	 */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Ammo")
	TArray<FAmmoCount> AmmunitionLimit;

protected:
	
	virtual void BeginPlay() override;

public:	
	
	//virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	template <class ThisType>
	ThisType* GetThisType() const
	{
		TArray<UActorComponent*> myComponents = GetOwner()->GetComponents().Array();
		ThisType* returnedVal = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			returnedVal = Cast<ThisType>(myComponents[i]);
			if (returnedVal) break;
		}
		return returnedVal;
	}

	URangedWeaponComponent* RangedWeaponComponent;

	bool IsAmmoEnough();
	bool IsAmmoEnough(const EAmmoType InAmmoType);
	bool IsAmmoEnough(const EEnergyType InEnergyType);
	bool IsAmmoEnough(const int32 InAmmo);
	bool IsAmmoEnough(const float MyEnergy, const float MyEnergyPerShot);
	bool IsWeaponNotOverheating();

	//=======================
	// Weapon stat (private):
	//=======================

	FAmmoCount AmmunitionCount;
	FExternalEnergyCount EnergyExternal;
		
};
