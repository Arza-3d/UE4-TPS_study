// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

//=============================================================================
/**
 *  UHPandMPComponent contain character stat that can be use by Actor
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_STUDY_API UHPandMPComponent : public UActorComponent
{
	GENERATED_BODY()

//===========================================================================
public:	
//===========================================================================
	
	UHPandMPComponent();

//===========================================================================
protected:
//===========================================================================

	virtual void BeginPlay() override;

	UPROPERTY()
	int32 CharHP = 100;

	UPROPERTY()
	int32 CharMP = 100;

//===========================================================================
private:
//===========================================================================

	APawn* TheChar;
};
