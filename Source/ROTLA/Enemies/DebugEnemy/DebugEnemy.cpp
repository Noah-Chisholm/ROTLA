// Fill out your copyright notice in the Description page of Project Settings.

#include "DebugEnemy.h"
#include "ROTLA/Utility/GI_ROTLA/GI_ROTLA.h"
#include "ROTLA/Enemies/EnemyRegistry.h"

// Sets default values
ADebugEnemy::ADebugEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	EnemyFlags = NewObject<UFlag>();
	CurHealth = MaxHealth;
	SetFlag(EEntityFlags::IsAlive);
}

FString ADebugEnemy::GetHealthString()
{
	return FString::Printf(TEXT("%f / %f"), CurHealth, MaxHealth);
}

float ADebugEnemy::GetHealthPercent()
{
	return CurHealth / MaxHealth;
}

// Called when the game starts or when spawned
void ADebugEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

bool ADebugEnemy::DealDamage(FDamage ToDeal)
{
	TakeDamage(ToDeal.Amount);
	switch (ToDeal.Type)
	{
	case EDamageTypes::Burning:
		SetFlag(EEntityFlags::IsOnFire);
		BurnTicks += 3;
		if (!BurnTimer.IsValid()) {
			GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &ADebugEnemy::HandleBurning, 1.0f);
		}
		break;
	case EDamageTypes::Corrosion:
		SetFlag(EEntityFlags::IsCorroding);
		CorrodeTicks += 3;
		if (!CorrodeTimer.IsValid()) {
			GetWorld()->GetTimerManager().SetTimer(CorrodeTimer, this, &ADebugEnemy::HandleCorrosion, 1.0f);
		}
		break;
	default:
		break;
	}
	return false;
}

void ADebugEnemy::HandleBurning() {
	if (HasFlag(EEntityFlags::IsOnFire)) {
		TakeDamage(BurnTicks * BurnMult);
		BurnTicks--;
		if (BurnTicks != 0) {
			GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &ADebugEnemy::HandleBurning, 1.0f);
		}
		else {
			RemoveFlag(EEntityFlags::IsOnFire);
		}
	}
}

bool ADebugEnemy::TakeDamage(float Damage)
{
	CurHealth -= Damage;
	if (CurHealth <= 0) {
		Die();
		return false;
	}
	return true;
}

void ADebugEnemy::Die() {
	SetFlag(EEntityFlags::IsDead);
	RemoveFlag(EEntityFlags::IsAlive);
	this->Destroy();
}

void ADebugEnemy::HandleCorrosion() {
	if (HasFlag(EEntityFlags::IsCorroding)) {
		TakeDamage(CorrodeTicks * CorrosionMult);
		CorrodeTicks--;
		if (CorrodeTicks != 0) {
			GetWorld()->GetTimerManager().SetTimer(BurnTimer, this, &ADebugEnemy::HandleCorrosion, 1.0f);
		}
		else {
			RemoveFlag(EEntityFlags::IsCorroding);
		}
	}
}

// Called every frame
void ADebugEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



// Called to bind functionality to input
void ADebugEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

