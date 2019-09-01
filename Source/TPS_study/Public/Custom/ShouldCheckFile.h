#pragma once

#include "CoreMinimal.h"

#define SHOULDCHECKFILE false
#define SHOULDNOTCHECKFILEFORSTRUCT false // set to TRUE to AVOID CRASH if you wanna build while editor is on

class TPS_STUDY_API ShouldCheckFile
{
public:
	ShouldCheckFile();
	~ShouldCheckFile();
};
