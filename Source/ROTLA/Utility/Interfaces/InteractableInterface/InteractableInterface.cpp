#include "InteractableInterface.h"

bool IInteractableInterface::StartPrimaryInteraction() {
	return true;
}

bool IInteractableInterface::FinishPrimaryInteraction() {
	return true;
}

bool IInteractableInterface::SecondaryInteraction() {
	return true;
}

bool IInteractableInterface::TertiaryInteraction() {
	return true;
}

void* IInteractableInterface::InteractableInteraction(void* Data, bool& Success) {
	Success = true;
	return nullptr;
}