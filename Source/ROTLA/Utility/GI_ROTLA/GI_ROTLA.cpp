// Fill out your copyright notice in the Description page of Project Settings.

#include "GI_ROTLA.h"
#include "Widgets/SWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "ROTLA/Utility/DebugCommandHandler/DebugCommandHandler.h"
#include "ROTLA/Weapons/WeaponRegistry.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/SWeakWidget.h"

void UGI_ROTLA::OpenDebugWindow(UUserWidget* Widget) {

	if (!Widget) return;

	if (!DebugWindow.IsValid())
	{
		DebugWindow = SNew(SWindow)
			.Title(FText::FromString("Debug Window"))
			.ClientSize(FVector2D(400, 300))
			.SupportsMinimize(true)
			.SupportsMaximize(false)
			.SizingRule(ESizingRule::UserSized)
			.HasCloseButton(false);

		FSlateApplication::Get().AddWindow(DebugWindow.ToSharedRef());
	}

	DebugWindow->SetContent(Widget->TakeWidget());

	DebugHandler = NewObject<UDebugCommandHandler>(this, UDebugCommandHandler::StaticClass());
	DebugHandler->Init(this);
}

void UGI_ROTLA::Shutdown()
{
	Super::Shutdown();

	// Also clear here just in case
	CloseDebugWindow();
}

void UGI_ROTLA::CloseDebugWindow()
{
	if (DebugWindow.IsValid())
	{
		DebugWindow->RequestDestroyWindow();
		DebugWindow.Reset();
	}
}

void UGI_ROTLA::Init() {
	IsEditor = GIsEditor && !IsRunningCommandlet();
	Super::Init();
}
