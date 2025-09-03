// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "ROTLA/Utility/Flag.h"
#include "ROTLA/Utility/EntityFlags.h"
#include "PlayerPawn.generated.h"

USTRUCT(BlueprintType)
struct FHUDData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString AmmoDisplay;
};

UCLASS()
class ROTLA_API APC_ROTLA : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APC_ROTLA();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);

	void Look(const FInputActionValue& Value);

	void JumpROTLA(const FInputActionValue& Value);

	void CrouchHandler(const FInputActionValue& Value);

	void CrouchROTLA();

	void HandleBack(const FInputActionValue& Value);

	void SprintHandler(const FInputActionValue& Value);

	void Sprint();

	void Slide();

	void StopMove();

	void StopSprint();
	
	void StopCrouch();

	void StopSlide();

	void StopJump();

	void StopJumping();

	void HandleStartPrimaryAction();
	
	void HandleFinishPrimaryAction();

	void HandleSecondaryAction();

	void HandleTertiarayAction();

	void HandleInteractionAction();

	void HandleReload();

	void ChangeHeight(float DesiredHeight);

	bool UnequipWeapon();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool EquipWeapon(ADebugWeapon* Weapon);

	virtual bool EquipWeapon_Implementation(ADebugWeapon* Weapon);

	UFUNCTION(BlueprintImplementableEvent)
	void ExitWidget();

	bool CanJumpInternal_Implementation() const override;

	UFlag* PlayerFlag;
	
	UFUNCTION(BlueprintCallable)
	void SetFlag(EEntityFlags Value) {
		PlayerFlag->_SetFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	void RemoveFlag(EEntityFlags Value) {
		PlayerFlag->_RemoveFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	bool HasFlag(EEntityFlags Value) const {
		return PlayerFlag->_HasFlag(static_cast<int32>(Value));
	};

	UFUNCTION(BlueprintCallable)
	FHUDData GetHudData();

	virtual void Landed(const FHitResult& Hit) override;

	FTimerHandle MovingTimer;
	float NormalHeight;
	float CrouchHeight;
	float SlideHeight;
	float CrouchSpeed = 300.0f;
	float WalkSpeed = 600.0f;
	float SprintSpeed = 900.0f;
	float SlideBoost = 1250.0f;
	float SlideDuration = 2.0f;
	float SlideFriction = 0.1f;
	float NormalFriction;
	float JumpHeight = 550.0f;
	float HeightChangeSpeed = 1.0f;
	float DesiredHeight;
	float AirControl = 0.0f;
	int32 MaxJumps = 1;
	float MaxJumpTime = 0.5f;
	float SlideTime = 0.0f;
	FVector SlideDirection;
	float InteractionDistance = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* BackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* PrimaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SecondaryAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* TertiararyAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* InteractableAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* ReloadAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons")
	class USceneComponent* PrimaryGripLocation;

	class ADebugWeapon* EquipedWeapon;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	friend class UDebugCommandHandler;
};
