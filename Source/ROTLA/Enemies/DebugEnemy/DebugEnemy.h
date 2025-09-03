// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ROTLA/Utility/Interfaces/DamageInterface/DamageInterface.h"
#include "ROTLA/Utility/Flag.h"
#include "ROTLA/Utility/EntityFlags.h"
#include "DebugEnemy.generated.h"


UCLASS()
class ROTLA_API ADebugEnemy : public ACharacter, public IDamageInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADebugEnemy();

	UFUNCTION(BlueprintCallable)
	FString GetHealthString();

	UFUNCTION(BlueprintCallable)
	float GetHealthPercent();
protected:
	FName EnemyName = "DefaultEnemy";
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual bool DealDamage(FDamage ToDeal) override;
	virtual void HandleCorrosion();
	virtual void HandleBurning();
	virtual bool TakeDamage(float Damage);
	virtual void Die();

	FTimerHandle BurnTimer;
	FTimerHandle CorrodeTimer;

	float CurHealth;
	int32 BurnTicks = 0;
	int32 CorrodeTicks = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxHealth = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CorrosionMult = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BurnMult = 1.0f;

	UFlag* EnemyFlags;

	UFUNCTION(BlueprintCallable)
	void SetFlag(EEntityFlags Value) {
		EnemyFlags->_SetFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	void RemoveFlag(EEntityFlags Value) {
		EnemyFlags->_RemoveFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	bool HasFlag(EEntityFlags Value) const {
		return EnemyFlags->_HasFlag(static_cast<int32>(Value));
	};

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
