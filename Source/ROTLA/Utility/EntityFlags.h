#pragma once

#include "CoreMinimal.h"
#include "EntityFlags.Generated.h"

UENUM(BlueprintType)
enum class EEntityFlags : uint8
{
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
	WantsToJump,
	IsFiring,
	IsCorroding,
	IsOnFire
};