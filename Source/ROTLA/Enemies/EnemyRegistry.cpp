#include "EnemyRegistry.h"
#include "Engine/AssetManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "UObject/SoftObjectPath.h"
#include "UObject/SoftObjectPtr.h"
#include "DebugEnemy/DebugEnemy.h"

UEnemyRegistry::UEnemyRegistry() {
    
}

#include "Engine/AssetManager.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/Blueprint.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/SoftObjectPath.h"
#include "DebugEnemy/DebugEnemy.h"

void UEnemyRegistry::CreateRegistry()
{
    IAssetRegistry& AR = UAssetManager::Get().GetAssetRegistry();
    //AR.WaitForCompletion(); // safe in editor; no-op in cooked

    // 1) All classes derived from ADebugEnemy (any depth)
    TArray<FTopLevelAssetPath> Base;
    Base.Add(ADebugEnemy::StaticClass()->GetClassPathName());

    TSet<FTopLevelAssetPath> Derived; // result
    AR.GetDerivedClassNames(Base, TSet<FTopLevelAssetPath>{}, Derived);

    // 2) Gather Blueprint assets under /Game/Enemies
    FARFilter Filter;
    Filter.PackagePaths.Add(TEXT("/Game/Enemies"));
    Filter.bRecursivePaths = true;
    Filter.ClassPaths.Add(UBlueprint::StaticClass()->GetClassPathName());
    Filter.bRecursiveClasses = true;

    TArray<FAssetData> Assets;
    AR.GetAssets(Filter, Assets);

    RegistryMap.Empty();
    RegistryMap.Reserve(Assets.Num());
    // 3) Keep only BPs whose generated class is in Derived (type-safe comparison)
    for (const FAssetData& A : Assets)
    {
#if ENGINE_MAJOR_VERSION >= 5
        FString GeneratedClassStr;
        if (!A.GetTagValue(FBlueprintTags::GeneratedClassPath, GeneratedClassStr) || GeneratedClassStr.IsEmpty())
            continue;
#else
        FString GeneratedClassStr;
        if (!A.GetTagValue(FName(TEXT("GeneratedClass")), GeneratedClassStr) || GeneratedClassStr.IsEmpty())
            continue;
#endif

        // Build typed paths from the string (no loads)
        FSoftObjectPath GeneratedClassSOP(GeneratedClassStr);
        const FTopLevelAssetPath GeneratedTop = GeneratedClassSOP.GetAssetPath();

        // Keep only if this BP’s generated class is a descendant of ADebugEnemy
        if (!Derived.Contains(GeneratedTop))
            continue;

        const FName Id = A.AssetName; // e.g. BP_SlimeElite
        if (!RegistryMap.Contains(Id))
        {
            TSoftClassPtr<ADebugEnemy> SoftClass(GeneratedClassSOP);
            RegistryMap.Add(Id, SoftClass);
        }
    }
}

bool UEnemyRegistry::GetFromName(FName Name, TSoftClassPtr<ADebugEnemy>& ClassToReturn) {
	if (RegistryMap.Find(Name)) {
		ClassToReturn = RegistryMap[Name];
		return true;
	}
	return false;
}

bool UEnemyRegistry::IsKey(FName Name) {
	return RegistryMap.Find(Name) != nullptr;
}

TArray<FName> UEnemyRegistry::GetEntries() {
	TArray<FName> ToReturn;
	RegistryMap.GetKeys(ToReturn);
	return ToReturn;
}