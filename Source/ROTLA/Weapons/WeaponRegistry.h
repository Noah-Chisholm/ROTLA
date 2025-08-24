#pragma once

#include "CoreMinimal.h"
#include "WeaponData.h"
#include "WeaponRegistry.generated.h"

UCLASS(BlueprintType)
class UWeaponRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public: 
	UWeaponRegistry();

	UFUNCTION(BlueprintCallable)
	TArray<FName> GetEntries();
	
	UFUNCTION(BlueprintCallable)
	bool GetFromName(FName Name, FWeaponData& VarToPopulate);

	UFUNCTION(BlueprintCallable)
	bool IsKey(FName Name);
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, FWeaponData> RegistryMap;
private:
};