// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerPawn.h"
#include "EnhancedInput/Public/EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "ROTLA/Utility/Interfaces/InteractableInterface/InteractableInterface.h"
#include "ROTLA/Weapons/DebugWeapon.h"
#include "EnhancedInputSubsystems.h"
#include "Components/CapsuleComponent.h"
#include "ROTLA/Utility/GI_ROTLA/GI_ROTLA.h"
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
	PrimaryGripLocation = CreateDefaultSubobject<USceneComponent>("PrimaryGripLocation");
	PrimaryGripLocation->SetupAttachment(FirstPersonCamera);
	PrimaryGripLocation->SetUsingAbsoluteRotation(false);
	PrimaryGripLocation->SetRelativeRotation(FRotator::ZeroRotator);

	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	PlayerFlag = NewObject<UFlag>();

	GetCharacterMovement()->bOrientRotationToMovement = false;

}

// Called when the game starts or when spawned
void APC_ROTLA::BeginPlay()
{	
	Super::BeginPlay();
	SetFlag(EEntityFlags::IsAlive);
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
		RemoveFlag(EEntityFlags::IsMoving);
		return;
	}
	SetFlag(EEntityFlags::IsMoving);
	FRotator ControlRot = Controller->GetControlRotation();
	FRotationMatrix YawMatrix(FRotator(0.f, ControlRot.Yaw, 0.f));

	FVector Forward = YawMatrix.GetUnitAxis(EAxis::X);
	FVector Right = YawMatrix.GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.X);
	AddMovementInput(Right, Axis.Y);

	GetWorld()->GetTimerManager().SetTimer(MovingTimer, this, &APC_ROTLA::StopMove, 0.1);
}

void APC_ROTLA::StopMove() {
	RemoveFlag(EEntityFlags::IsMoving);
}

void APC_ROTLA::StopSprint() {
	RemoveFlag(EEntityFlags::WantsToSprint);
	RemoveFlag(EEntityFlags::IsSprinting);
	if (HasFlag(EEntityFlags::IsCrouching))
		return;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APC_ROTLA::StopCrouch() {
	RemoveFlag(EEntityFlags::WantsToCrouch);
	if (HasFlag(EEntityFlags::IsSliding)) {
		//PRINT("STOPPING SLIDE FROM CROUCH!", MessageTypes::Message);
		StopSlide();
		return;
	}
	RemoveFlag(EEntityFlags::IsCrouching);
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	ChangeHeight(NormalHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(NormalHeight);
}

void APC_ROTLA::StopSlide() {
	//PRINT("STOPPING SLIDE!", MessageTypes::Message);
	RemoveFlag(EEntityFlags::IsSliding);
	RemoveFlag(EEntityFlags::WantsToSlide);
	auto* Move = GetCharacterMovement();
	Move->BrakingFrictionFactor = 1.0f;
	Move->GroundFriction = NormalFriction;
	ChangeHeight(NormalHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(NormalHeight);
	SlideTime = 0.0f;
	if (HasFlag(EEntityFlags::WantsToSprint))
		Sprint();
	else if (!HasFlag(EEntityFlags::IsJumping))
		CrouchROTLA();
}

void APC_ROTLA::StopJump() {
	Super::StopJumping();
}

void APC_ROTLA::StopJumping() {
 	RemoveFlag(EEntityFlags::WantsToJump);
	StopJump();
}

void APC_ROTLA::HandleStartPrimaryAction() {
	if (EquipedWeapon) {
		SetFlag(EEntityFlags::IsFiring);
		Cast<IInteractableInterface>(EquipedWeapon)->StartPrimaryInteraction();
	}
}

void APC_ROTLA::HandleFinishPrimaryAction() {
	if (EquipedWeapon) {
		RemoveFlag(EEntityFlags::IsFiring);
		Cast<IInteractableInterface>(EquipedWeapon)->FinishPrimaryInteraction();
	}
}

void APC_ROTLA::HandleSecondaryAction() {
}

void APC_ROTLA::HandleTertiarayAction() {
}

void APC_ROTLA::HandleInteractionAction() {
	FHitResult Hit;
	FVector StartLocation = FirstPersonCamera->GetComponentLocation();
	FVector EndLocation = FirstPersonCamera->GetForwardVector() * InteractionDistance + StartLocation;
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Camera, ColParams);
	auto* Actor = Hit.GetActor();
	IInteractableInterface* InteractableActor = Cast<IInteractableInterface>(Actor);
	if (InteractableActor) {
		bool SuccesfulInteraction;
		void* ReturnedData = InteractableActor->InteractableInteraction(nullptr, SuccesfulInteraction);
		ADebugWeapon* Weapon = Cast<ADebugWeapon>(Actor);
		if (Weapon) {
			EquipWeapon(static_cast<ADebugWeapon*>(ReturnedData));
		}
	}
}

void APC_ROTLA::HandleReload() {
	//TODO: Add Check for if the player has enough ammo;
	EquipedWeapon->ReloadAmmo(999);
}

void APC_ROTLA::ChangeHeight(float NewHeight) {
	DesiredHeight = NewHeight;
	SetFlag(EEntityFlags::IsChangingHeight);
}

bool APC_ROTLA::UnequipWeapon()
{
	if (!EquipedWeapon)
		return false;
	FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, true);
	EquipedWeapon->SetActorEnableCollision(true);
	EquipedWeapon->DetachFromActor(Rules);
	EquipedWeapon = nullptr;
	return true;
}

bool APC_ROTLA::EquipWeapon_Implementation(ADebugWeapon* Weapon)
{
	UnequipWeapon();
	EquipedWeapon = Weapon;
	EquipedWeapon->SetActorEnableCollision(false);
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	EquipedWeapon->AttachToActor(this, Rules);
	EquipedWeapon->PrimararyGripLocation->AttachToComponent(PrimaryGripLocation, Rules);
	return true;
}

void APC_ROTLA::Look(const FInputActionValue& Value) {
	auto Axis = Value.Get<FVector2D>();
	if (Axis.IsNearlyZero())
		return;
	AddControllerPitchInput(-Axis.Y);
	AddControllerYawInput(Axis.X);
}

bool APC_ROTLA::CanJumpInternal_Implementation() const {
	if (HasFlag(EEntityFlags::IsSliding))
		return true;
	else return Super::CanJumpInternal_Implementation();
}

void APC_ROTLA::JumpROTLA(const FInputActionValue& Value) {
	SetFlag(EEntityFlags::WantsToJump);
	if (CanJump())
		SetFlag(EEntityFlags::IsJumping);
	else
		return;
	StopSlide();
	Jump();
}

void APC_ROTLA::CrouchHandler(const FInputActionValue& Value) {
	if (HasFlag(EEntityFlags::IsJumping)) {
		if(HasFlag(EEntityFlags::WantsToSprint))
			SetFlag(EEntityFlags::WantsToSlide);
		return;
	}
	if (HasFlag(EEntityFlags::IsSprinting)) {
		Slide();
	}
	else {
		CrouchROTLA();
	}
}

void APC_ROTLA::Slide() {
	SetFlag(EEntityFlags::IsSliding);
	auto* Move = GetCharacterMovement();
	Move->GroundFriction = SlideFriction;
	Move->BrakingFrictionFactor = 0.0f;
	SlideDirection = Move->GetLastUpdateVelocity();
	SlideDirection.Normalize();
	SlideDirection.Z = 0.0f;
	ChangeHeight(SlideHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(SlideHeight);
	LaunchCharacter(SlideDirection * SlideBoost, true, false);
}

void APC_ROTLA::CrouchROTLA() {
	SetFlag(EEntityFlags::WantsToCrouch);
	if (HasFlag(EEntityFlags::IsJumping)) {
		return;
	}
	SetFlag(EEntityFlags::IsCrouching);
	GetCharacterMovement()->MaxWalkSpeed = CrouchSpeed;
	ChangeHeight(CrouchHeight);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(CrouchHeight);
}

void APC_ROTLA::SprintHandler(const FInputActionValue& Value) {
	Sprint();
}

void APC_ROTLA::Sprint() {
	SetFlag(EEntityFlags::WantsToSprint);
	if (HasFlag(EEntityFlags::IsJumping)) {
		return;
	}
	SetFlag(EEntityFlags::IsSprinting);
	//if (HasFlag(EEntityFlags::IsSliding)) {
	//	PRINTM("Stopping due to sprint");
	//	StopSlide();
	//}
	if (HasFlag(EEntityFlags::IsCrouching))
		StopCrouch();
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APC_ROTLA::HandleBack(const FInputActionValue& Value) {
	bool IsInWidget = HasFlag(EEntityFlags::IsInWidget);
	if (IsInWidget)
		ExitWidget();
	return;
}

FHUDData APC_ROTLA::GetHudData()
{
	FHUDData DataToReturn;
	DataToReturn.AmmoDisplay = EquipedWeapon ? EquipedWeapon->GetAmmoString() : "";
	return DataToReturn;
}

void APC_ROTLA::Landed(const FHitResult& Result) {
	Super::Landed(Result);
	RemoveFlag(EEntityFlags::IsJumping);
	if (HasFlag(EEntityFlags::WantsToJump))
		JumpROTLA(FInputActionValue());
	else if (HasFlag(EEntityFlags::WantsToSlide)) {
		Slide();
	}
	else if (HasFlag(EEntityFlags::WantsToSprint))
		Sprint();
	else if (HasFlag(EEntityFlags::WantsToCrouch))
		CrouchROTLA();
}

// Called every frame
void APC_ROTLA::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (HasFlag(EEntityFlags::IsSliding)) {
		LaunchCharacter(SlideDirection * SlideBoost, true, false);
		SlideTime += DeltaTime;
		//PRINT("SLIDE TIME: " + FString::SanitizeFloat(SlideTime), MessageTypes::Message);
		if (SlideTime >= SlideDuration) {
			//PRINT("STOPPING SLIDE IN TICK!", MessageTypes::Message);
			StopSlide();
		}
		if (HasFlag(EEntityFlags::WantsToJump))
			Jump();
	}
	if (HasFlag(EEntityFlags::IsChangingHeight)) {
		auto* Cap = GetCapsuleComponent();
		float CurHeight = Cap->GetUnscaledCapsuleHalfHeight();
		float Height = CurHeight < DesiredHeight ? CurHeight + HeightChangeSpeed : CurHeight - HeightChangeSpeed;
		//PRINTM("DHEIGHT: " + FString::SanitizeFloat(DesiredHeight));
		//PRINTM("HEIGHT: " + FString::SanitizeFloat(Height));
		if (FMath::IsNearlyEqual(Height, DesiredHeight, 1)) {
			Cap->SetCapsuleHalfHeight(DesiredHeight);
			RemoveFlag(EEntityFlags::IsChangingHeight);
		} else Cap->SetCapsuleHalfHeight(Height);
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
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopJumping);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &APC_ROTLA::CrouchHandler);
	EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopCrouch);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &APC_ROTLA::SprintHandler);
	EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APC_ROTLA::StopSprint);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Started, this, &APC_ROTLA::HandleStartPrimaryAction);
	EnhancedInputComponent->BindAction(PrimaryAction, ETriggerEvent::Completed, this, &APC_ROTLA::HandleFinishPrimaryAction);
	EnhancedInputComponent->BindAction(SecondaryAction, ETriggerEvent::Completed, this, &APC_ROTLA::HandleSecondaryAction);
	EnhancedInputComponent->BindAction(TertiararyAction, ETriggerEvent::Completed, this, &APC_ROTLA::HandleTertiarayAction);
	EnhancedInputComponent->BindAction(InteractableAction, ETriggerEvent::Completed, this, &APC_ROTLA::HandleInteractionAction);
	EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APC_ROTLA::HandleReload);
	
	EnhancedInputComponent->BindAction(BackAction, ETriggerEvent::Triggered, this, &APC_ROTLA::HandleBack);
	
}

