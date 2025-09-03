// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityObject.h"
#include "TagModifier.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ROTLAEDITORONLY_API UTagModifier : public UEditorUtilityObject
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable)
	void AddMetaDataTag(FName Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveMetaDataTag(FName Tag);

	UFUNCTION(BlueprintCallable)
	TSet<FName> GetTags();
};