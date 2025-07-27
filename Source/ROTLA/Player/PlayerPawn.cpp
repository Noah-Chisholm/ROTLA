// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include <ROTLA/Utility/GI_ROTLA/GI_ROTLA.h>
#include "EnhancedInput/Public/InputMappingContext.h"

// Sets default values
APC_ROTLA::APC_ROTLA()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Player0;
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
	FirstPersonCamera->bUsePawnControlRotation = true;
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;

}

// Called when the game starts or when spawned
void APC_ROTLA::BeginPlay()
{
	Super::BeginPlay();
	SetFlag(EPlayerFlags::IsAlive);
	auto* Move = GetCharacterMovement();
	Move->MaxWalkSpeedCrouched = CrouchSpeed;
	NormalFriction = Move->GroundFriction;
	NormalHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	CrouchHeight = NormalHeight / 2;
	Move->SetCrouchedHalfHeight(CrouchHeight);
	SlideHeight = CrouchHeight / 2;
	Move->JumpZVelocity = JumpHeight;
	ACharacter::JumpMaxCount = APC_ROTLA::MaxJumps;
	ACharacter::JumpMaxHoldTime = APC_ROTLA::MaxJumpTime;
	Move->AirControl = AirControl;
}

void APC_ROTLA::Move(const FInputActionValue& Value)
{
	auto Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero())
	{
		RemoveFlag(EPlayerFlags::IsMoving);
		return;
	}
	SetFlag(EPlayerFlags::IsMoving);
	FRotator ControlRot = Controller->GetControlRotation();
	FRotationMatrix YawMatrix(FRotator(0.f, ControlRot.Yaw, 0.f));

	FVector Forward = YawMatrix.GetUnitAxis(EAxis::X);
	FVector Right = YawMatrix.GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.X);
	AddMovementInput(Right, Axis.Y);

	GetWorld()->GetTimerManager().SetTimer(MovingTimer, this, &APC_ROTLA::StopMove, 0.1);
}

void APC_ROTLA::StopMove() {
	RemoveFlag(EPlayerFlags::IsMoving);
}

void APC_ROTLA::StopSprint() {
	RemoveFlag(EPlayerFlags::WantsToSprint);
	RemoveFlag(EPlayerFlags::IsSprinting);
	if (HasFlag(EPlayerFlags::IsCrouching))
		return;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APC_ROTLA::StopCrouch() {
	RemoveFlag(EPlayerFlags::WantsToCrouch);
	if (HasFlag(EPlayerFlags::IsSliding)) {
		//PRINT("STOPPING SLIDE FROM CROUCH!", MessageTypes::Message);
		StopSlide();
		return;
	}
	RemoveFlag(EPlayerFlags::IsCrouching);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ChangeHeight(NormalHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(NormalHeight);
}

void APC_ROTLA::StopSlide() {
	//PRINT("STOPPING SLIDE!", MessageTypes::Message);
	RemoveFlag(EPlayerFlags::IsSliding);
	RemoveFlag(EPlayerFlags::WantsToSlide);
	auto* Move = GetCharacterMovement();
	Move->BrakingFrictionFactor = 1.0f;
	Move->GroundFriction = NormalFriction;
	ChangeHeight(NormalHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(NormalHeight);
	SlideTime = 0.0f;
	if (HasFlag(EPlayerFlags::WantsToSprint))
		Sprint();
	else
		CrouchROTLA();
}

void APC_ROTLA::StopJump() {
	Super::StopJumping();
}

void APC_ROTLA::ChangeHeight(float NewHeight) {
	DesiredHeight = NewHeight;
	SetFlag(EPlayerFlags::IsChangingHeight);
}

void APC_ROTLA::Look(const FInputActionValue& Value) {
	auto Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero())
		return;
	AddControllerPitchInput(-Axis.Y);
	AddControllerYawInput(Axis.X);
}

bool APC_ROTLA::CanJumpInternal_Implementation() const {
	if (HasFlag(EPlayerFlags::IsSliding))
		return true;
	else return Super::CanJumpInternal_Implementation();
}

void APC_ROTLA::JumpROTLA(const FInputActionValue& Value) {
	SetFlag(EPlayerFlags::IsJumping);
	SetFlag(EPlayerFlags::WantsToJump);
	StopSlide();
	Jump();
}

void APC_ROTLA::CrouchHandler(const FInputActionValue& Value) {
	if (HasFlag(EPlayerFlags::IsJumping)) {
		if(HasFlag(EPlayerFlags::WantsToSprint))
			SetFlag(EPlayerFlags::WantsToSlide);
		return;
	}
	if (HasFlag(EPlayerFlags::IsSprinting)) {
		Slide();
	}
	else {
		CrouchROTLA();
	}
}

void APC_ROTLA::Slide() {
	SetFlag(EPlayerFlags::IsSliding);
	auto* Move = GetCharacterMovement();
	Move->GroundFriction = SlideFriction;
	Move->BrakingFrictionFactor = 0.0f;
	SlideDirection = GetActorForwardVector();
	ChangeHeight(SlideHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(SlideHeight);
	LaunchCharacter(GetActorForwardVector() * SlideBoost, true, false);
}

void APC_ROTLA::CrouchROTLA() {
	SetFlag(EPlayerFlags::WantsToCrouch);
	if (HasFlag(EPlayerFlags::IsJumping)) {
		return;
	}
	SetFlag(EPlayerFlags::IsCrouching);
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	ChangeHeight(CrouchHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchHeight);
}

void APC_ROTLA::SprintHandler(const FInputActionValue& Value) {
	Sprint();
}

void APC_ROTLA::Sprint() {
	SetFlag(EPlayerFlags::WantsToSprint);
	if (HasFlag(EPlayerFlags::IsJumping)) {
		return;
	}
	SetFlag(EPlayerFlags::IsSprinting);
	//if (HasFlag(EPlayerFlags::IsSliding)) {
	//	PRINTM("Stopping due to sprint");
	//	StopSlide();
	//}
	if (HasFlag(EPlayerFlags::IsCrouching))
		StopCrouch();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APC_ROTLA::HandleBack(const FInputActionValue& Value) {
	bool IsInWidget = HasFlag(EPlayerFlags::IsInWidget);
	if (IsInWidget)
		ExitWidget();
	return;
}

void APC_ROTLA::SetFlag(EPlayerFlags Flag) {
	PlayerFlags |= 1 << static_cast<int32>(Flag);
}

void APC_ROTLA::RemoveFlag(EPlayerFlags Flag) {
	//if (Flag == EPlayerFlags::IsSliding)
		//PRINT("Removing Sliding Flag?", MessageTypes::Message);
	PlayerFlags &= ~(1 << static_cast<int32>(Flag));
}

bool APC_ROTLA::HasFlag(EPlayerFlags Flag) const {
	return PlayerFlags & (1 << static_cast<int32>(Flag));
}

void APC_ROTLA::Landed(const FHitResult& Result) {
	Super::Landed(Result);
	RemoveFlag(EPlayerFlags::IsJumping);
	if (HasFlag(EPlayerFlags::WantsToSlide)) {
		Slide();
	}
	else if (HasFlag(EPlayerFlags::WantsToSprint))
		Sprint();
	else if (HasFlag(EPlayerFlags::WantsToCrouch))
		CrouchROTLA();
}

// Called every frame
void APC_ROTLA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasFlag(EPlayerFlags::IsSliding)) {
		LaunchCharacter(SlideDirection * SlideBoost, true, false);
		SlideTime += DeltaTime;
		//PRINT("SLIDE TIME: " + FString::SanitizeFloat(SlideTime), MessageTypes::Message);
		if (SlideTime >= SlideDuration) {
			//PRINT("STOPPING SLIDE IN TICK!", MessageTypes::Message);
			StopSlide();
		}
		if (HasFlag(EPlayerFlags::WantsToJump))
			Jump();
	}
	if (HasFlag(EPlayerFlags::IsChangingHeight)) {
		auto* Cap = GetCapsuleComponent();
		float CurHeight = Cap->GetUnscaledCapsuleHalfHeight();
		float Height = CurHeight < DesiredHeight ? CurHeight + HeightChangeSpeed : CurHeight - HeightChangeSpeed;
		//PRINTM("DHEIGHT: " + FString::SanitizeFloat(DesiredHeight));
		//PRINTM("HEIGHT: " + FString::SanitizeFloat(Height));
		if (FMath::IsNearlyEqual(Height, DesiredHeight, 1)) {
			Cap->SetCapsuleHalfHeight(DesiredHeight);
			RemoveFlag(EPlayerFlags::IsChangingHeight);
		} else Cap->SetCapsuleHalfHeight(Height);
	}
	if (HasFlag(EPlayerFlags::WantsToJump)) {
		if (!bWasJumping)
			Jump();
		else
			RemoveFlag(EPlayerFlags::WantsToJump);
	}
}

// Called to bind functionality to input
void APC_ROTLA::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	(Cast<APlayerController>(GetController()))->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(InputMappingContext, 0);
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APC_ROTLA::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APC_ROTLA::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APC_ROTLA::JumpROTLA);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopJump);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APC_ROTLA::CrouchHandler);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopCrouch);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APC_ROTLA::SprintHandler);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopSprint);
	
	EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &APC_ROTLA::HandleBack);
	
}

