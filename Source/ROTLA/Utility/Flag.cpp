#include "Flag.h"

UFlag::UFlag() {

}

void UFlag::_RemoveFlag(int32 FlagIn) {
	Flag &= ~(1 << FlagIn);
}

bool UFlag::_HasFlag(int32 FlagIn) const {
	return Flag & (1 << FlagIn);
}

void UFlag::_SetFlag(int32 FlagIn) {
	Flag |= 1 << FlagIn;
}
