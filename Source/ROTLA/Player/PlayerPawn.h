// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "PlayerPawn.generated.h"

UENUM(BlueprintType, meta = (Bitflags))
enum class EPlayerFlags : uint8
{
	None,
	IsAlive,
	IsDead,
	IsJumping,
	IsCrouching,
	IsSprinting,
	IsSliding,
	IsMoving,
	IsInWidget,
	WantsToSprint,
	WantsToSlide,
	WantsToCrouch,
	IsChangingHeight,
	WantsToJump
};
ENUM_CLASS_FLAGS(EPlayerFlags)


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

	void ChangeHeight(float DesiredHeight);

	UFUNCTION(BlueprintImplementableEvent)
	void ExitWidget();

	bool CanJumpInternal_Implementation() const override;

	UFUNCTION(BlueprintCallable)
	void SetFlag(EPlayerFlags Flag);

	UFUNCTION(BlueprintCallable)
	void RemoveFlag(EPlayerFlags Flag);

	UFUNCTION(BlueprintCallable)
	bool HasFlag(EPlayerFlags Flag) const;

	virtual void Landed(const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	int32 PlayerFlags;

	FTimerHandle MovingTimer;
	float NormalHeight;
	float CrouchHeight;
	float SlideHeight;
	float CrouchSpeed = 300.0f;
	float WalkSpeed = 600.0f;
	float SprintSpeed = 900.0f;
	float SlideBoost = 1000.0f;
	float SlideDuration = 2.0f;
	float SlideFriction = 0.1f;
	float NormalFriction;
	float JumpHeight = 1000.0f;
	float HeightChangeSpeed = 1.0f;
	float DesiredHeight;
	float AirControl = 0.0f;
	int32 MaxJumps = 1;
	float MaxJumpTime = 0.5f;
	float SlideTime = 0.0f;
	FVector SlideDirection;

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
	UInputAction* SprintAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	class UCameraComponent* FirstPersonCamera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
