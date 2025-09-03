// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "GI_ROTLA.generated.h"

UENUM(BlueprintType)
enum class MessageTypes : uint8
{
	Message,
	Warning,
	Error,
	Success
};

#define PRINT(message, type) Cast<UGI_ROTLA>(GetGameInstance())->PrintDebugC(FText::FromString(message), type);
#define PRINTM(message) PRINT(message, MessageTypes::Message);

UCLASS()
class ROTLA_API UGI_ROTLA : public UGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PrintDebugC(const FText& Message, MessageTypes Type);

	UFUNCTION(BlueprintCallable, Category = "Debug")
	void OpenDebugWindow(UUserWidget* Widget);

	UPROPERTY(BlueprintReadOnly)
	class UDebugCommandHandler* DebugHandler;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UWeaponRegistry* Weapons;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	class UEnemyRegistry* Enemies;

protected:
	void Shutdown() override;
	void CloseDebugWindow();
	void Init() override;

	UPROPERTY(BlueprintReadOnly)
	bool IsEditor = false;
private:
	TSharedPtr<class SWindow> DebugWindow;
};
