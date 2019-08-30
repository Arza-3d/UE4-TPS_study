#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "Struct/ProjectileParticleStruct.h"

#include "ProjectileFXDataAsset.generated.h"


/**
 * 
 */
UCLASS()
class TPS_STUDY_API UProjectileFXDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FProjectileParticle ProjectileVX;
};
