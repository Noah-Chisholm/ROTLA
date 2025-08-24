#include "WeaponRegistry.h"

UWeaponRegistry::UWeaponRegistry()
{
}

TArray<FName> UWeaponRegistry::GetEntries() {
	TArray<FName> ToReturn;
	RegistryMap.GetKeys(ToReturn);
	return ToReturn;
}

bool UWeaponRegistry::GetFromName(FName Name, FWeaponData& VarToPopulate) {
	if (RegistryMap.Find(Name)) {
		VarToPopulate = RegistryMap[Name];
		return true;
	}
	return false;
}

bool UWeaponRegistry::IsKey(FName Name) {
	return RegistryMap.Find(Name) != nullptr;
}