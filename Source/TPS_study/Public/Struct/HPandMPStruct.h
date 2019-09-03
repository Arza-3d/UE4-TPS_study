// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HPandMPStruct.generated.h"

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

/**
 * 
 */
UCLASS()
class TPS_STUDY_API UHPandMPStruct : public UObject
{
	GENERATED_BODY()
	
};
