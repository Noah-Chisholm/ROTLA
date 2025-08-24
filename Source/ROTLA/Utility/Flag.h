// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Flag.generated.h"

#define SETFLAG(FlagObject, EnumToSet) FlagObject->_SetFlag(static_cast<int32>(EnumToSet));
#define REMOVEFLAG(FlagObject, EnumToRemove) FlagObject->_RemoveFlag(static_cast<int32>(EnumToRemove));
#define HASFLAG(FlagObject, EnumToCheck) FlagObject->_HasFlag(static_cast<int32>(EnumToCheck));

#define IMPLEMENTFLAGFUNCTIONS(FlagObject, EnumType) void SetFlag(EnumType Value) { \
	SETFLAG(FlagObject, Value) \
}; \
void RemoveFlag(EnumType Value) { \
	REMOVEFLAG(FlagObject, Value) \
}; \
bool HasFlag(EnumType Value) const { \
	return HASFLAG(FlagObject, Value) \
}; \

UCLASS()
class ROTLA_API UFlag : public UObject
{
	GENERATED_BODY()

public: 
	UFlag();
	
	//Use the macro, these are internal
	UFUNCTION(BlueprintCallable)
	void _SetFlag(int32 Value);

	//Use the macro, these are internal
	UFUNCTION(BlueprintCallable)
	void _RemoveFlag(int32 Value);

	//Use the macro, these are internal
	UFUNCTION(BlueprintCallable)
	bool _HasFlag(int32 Value) const;

private:
	int32 Flag = 0;
};