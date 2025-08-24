#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ROTLA/Utility/GI_ROTLA/GI_ROTLA.h"
#include "ROTLA/Weapons/WeaponData.h"
#include "DebugCommandHandler.generated.h"

#define NewCommand(NAME) bool NAME(FJsonSerializableArray Command, FString& ErrorMessage, bool IsFinished);

UCLASS(BlueprintType)
class UDebugCommandHandler : public UObject
{
	GENERATED_BODY()

public:
	UDebugCommandHandler();

	UFUNCTION(BlueprintCallable)
	bool ProcessCommand(const FString& Command, FString& ErrorMessage, bool IsFinished);
	void Init(UGI_ROTLA* GI);
protected:
private:
	TArray<FName> CommandList = { "SetPlayerWeapon", "GivePlayerWeapon"};
	bool SetPlayerWeapon(FJsonSerializableArray Command, FString& ErrorMessage, bool IsFinished);
	NewCommand(GivePlayerWeapon)
	UGI_ROTLA* GameInstance;
	FString Invalid = " is not a valid ";
	FString FloatEx = "<float> ex: 3.14159";
	FString UIntEx = "<uint32> ex: x > 0; 1234";
};

