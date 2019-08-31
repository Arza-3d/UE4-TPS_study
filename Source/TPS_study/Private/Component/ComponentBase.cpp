#include "ComponentBase.h"

UComponentBase::UComponentBase()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UComponentBase::SetUpVariables(bool bShouldCheck)
{
}