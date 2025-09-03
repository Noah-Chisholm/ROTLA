// Fill out your copyright notice in the Description page of Project Settings.


#include "TagModifier.h"
#include "Engine/AssetManagerSettings.h"

void UTagModifier::AddMetaDataTag(FName Tag) {
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (!Settings->MetaDataTagsForAssetRegistry.Contains(Tag)) {
		Settings->MetaDataTagsForAssetRegistry.Add(Tag);
		Settings->SaveConfig();
		Settings->TryUpdateDefaultConfigFile();
		Settings->PostEditChange();
	}
}

void UTagModifier::RemoveMetaDataTag(FName Tag) {
	UAssetManagerSettings* Settings = GetMutableDefault<UAssetManagerSettings>();
	if (Settings->MetaDataTagsForAssetRegistry.Contains(Tag)) {
		Settings->MetaDataTagsForAssetRegistry.Remove(Tag);
		Settings->SaveConfig();
		Settings->TryUpdateDefaultConfigFile();
		Settings->PostEditChange();
	}
}

TSet<FName> UTagModifier::GetTags() {
	const UAssetManagerSettings* Settings = GetDefault<UAssetManagerSettings>();
	return Settings->MetaDataTagsForAssetRegistry;
}