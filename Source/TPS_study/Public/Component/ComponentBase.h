#pragma once

#include "CoreMinimal.h"
#include "UObject/ConstructorHelpers.h"

#include "Components/ActorComponent.h"
#include "Custom/ShouldCheckFile.h"
#include "ComponentBase.generated.h"

/**
 * base class for my blueprint component
 */
UCLASS()
class TPS_STUDY_API UComponentBase : public UActorComponent
{
	GENERATED_BODY()


//===========================================================================
public:
//===========================================================================

	UComponentBase();

//===========================================================================
protected:
//===========================================================================

	const bool bShouldDoCheckFile = SHOULDCHECKFILE;

	virtual void SetUpVariables(bool bShouldCheck);

	template <class ThisComponent>
	ThisComponent* GetComponentSibling() const
	{
		TArray<UActorComponent*> myComponents = GetOwner()->GetComponents().Array();
		ThisComponent* returnedVal = nullptr;

		for (int i = 0; i < myComponents.Num(); i++)
		{
			returnedVal = Cast<ThisComponent>(myComponents[i]);
			if (returnedVal) break;
		}
		return returnedVal;
	}

	template <class ThisFile>
	static ThisFile* GetThisFile(const TCHAR* ObjectToFind, bool bShouldCheck = true)
	{
		static ConstructorHelpers::FObjectFinder<ThisFile> inObj(ObjectToFind);
		if (bShouldCheck) check(inObj.Object);
		return inObj.Object;
	}	

};
