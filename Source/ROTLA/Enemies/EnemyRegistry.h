#pragma once

#include "CoreMinimal.h"
#include "DebugEnemy/DebugEnemy.h"
#include "EnemyRegistry.generated.h"

UCLASS(BlueprintType)
class UEnemyRegistry : public UObject
{
	GENERATED_BODY()

public: 
	UEnemyRegistry();

	void CreateRegistry();

	UFUNCTION(BlueprintCallable)
		TArray<FName> GetEntries();

	UFUNCTION(BlueprintCallable)
		bool GetFromName(FName Name, TSoftClassPtr<ADebugEnemy>& ClassToReturn);

	UFUNCTION(BlueprintCallable)
		bool IsKey(FName Name);
protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TMap<FName, TSoftClassPtr<ADebugEnemy>> RegistryMap;
};