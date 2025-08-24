#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

UINTERFACE()
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractableInterface
{
	GENERATED_BODY()

public:

	virtual bool StartPrimaryInteraction();

	virtual bool FinishPrimaryInteraction();

	virtual bool SecondaryInteraction();

	virtual bool TertiaryInteraction();

	virtual void* InteractableInteraction(void* DataIn, bool& Success);
};