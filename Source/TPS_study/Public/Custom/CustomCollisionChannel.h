#pragma once

#include "CoreMinimal.h"

#define ECC_PlayerProjectile	ECollisionChannel::ECC_GameTraceChannel1
#define ECC_Player				ECollisionChannel::ECC_GameTraceChannel2
#define ECC_Enemy				ECollisionChannel::ECC_GameTraceChannel3
#define ECC_EnemyProjectile		ECollisionChannel::ECC_GameTraceChannel4

class TPS_STUDY_API CustomCollisionChannel
{
public:
	CustomCollisionChannel();
	~CustomCollisionChannel();
};
