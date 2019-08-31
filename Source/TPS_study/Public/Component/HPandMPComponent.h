// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/ComponentBase.h"
#include "HPandMPComponent.generated.h"

USTRUCT(BlueprintType)
struct FCeiledFloat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat", Meta = (ClampMin = "0", ClampMax = "9999"))
	float Current = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat", Meta = (ClampMin = "1", ClampMax = "9999"))
	float Max = 100.0f;
};

USTRUCT(BlueprintType)
struct FCharacterStat
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	FCeiledFloat Health;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	FCeiledFloat Mana;
};

USTRUCT(BlueprintType)
struct FCharacterStatBPCPP
{
	GENERATED_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	float HP = 100.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Stat")
	float MP = 100.0f;
};

//=============================================================================
/**
 *  UHPandMPComponent contain character stat that can be use by Actor
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UHPandMPComponent : public UComponentBase
{
	GENERATED_BODY()

//===========================================================================
public:	
//===========================================================================
	
	UHPandMPComponent();

	friend class URangedWeaponComponent;

	//=================
	// Setter (public):
	//=================

	UFUNCTION(BlueprintCallable, Category = "Character Stat", Meta = (KeyWords = "heal damage health mana"))
	float AddHealth(const float AddHealth);

	UFUNCTION(BlueprintCallable, Category = "Character Stat", Meta = (KeyWords = "heal damage health mana"))
	float AddMana(const float AddMana);

	UFUNCTION(BlueprintCallable, Category = "Character Stat", Meta = (KeyWords = "heal damage health mana"))
	float SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Character Stat", Meta = (KeyWords = "heal damage health mana"))
	float SetMana(float NewMana);

	UFUNCTION(BlueprintCallable, Category = "Character Stat", Meta = (KeyWords = "heal damage health mana"))
	FCeiledFloat SetMaxHealth(const float NewMaxHealth);

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Character Stat", Meta = (DisplayPriority = "1"))
	FCharacterStat HealthAndMana;

//===========================================================================
private:
//===========================================================================

	//APawn* TheChar;

	float AddStat(float* CurrentStat, const float AddStat, const float MaxStat = 9999.0f);

	float SetStatClamped(float NewStat, const float MaxStat = 9999.0f);
};
