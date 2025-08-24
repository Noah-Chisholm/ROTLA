#pragma once

#include "CoreMinimal.h"
#include "DamageInterface.h"
#include "WeaponData.generated.h"

UENUM(BlueprintType)
enum class EProjectileType : uint8
{
	ROCKET,
	HOMINGROCKET,
	BULLET,
	HOMINGBULLET,
	BEAM,
	PLASMABOLT,
	ARROW
};

UENUM(BlueprintType)
enum class EFirePattern : uint8
{
	SINGLE,
	FULLAUTO,
	BURST
};

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_BODY()

public:
	FWeaponData() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float range = 10000.0f; //The Distance a projectile may travel before despawning
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	int32 ProjectileCount = 1; //The number of projectiles to spawn on firing
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	FDamage Damage = FDamage(EDamageTypes::Normal, 10.0f);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float accuracy = 0.0f; //The degrees of a cone that will decide trajectory
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	EProjectileType Projectile = EProjectileType::BULLET; //Projectile type; Affects Damage Type, Homing Stats, trajectory
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	int32 ClipSize = 25; // Max Ammo in clip
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	int32 StartingAmmo = 0; // starting ammo in clip on new spawn
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float ChargeTime = 0.001f; // Amount of time to wait before firing first shot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float ShotCoolDown = 0.5f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float HomingDuration = 0.001f; // Length of time that homing is active
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float HomingAccuracy = 0.0f; // The radius of a circle around the targeted entity that a homing projectile may strike
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	int32 BurstShots = 3; // The number of shots to fire in burst pattern
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float BurstDelay = 0.25f; // The delay between each shot in a burst

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	EFirePattern Pattern = EFirePattern::SINGLE; // the fire pattern
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	float ReloadTime = 3.0f; // The time to reload

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Firing")
	TSoftObjectPtr<USkeletalMesh> MeshToUse;
};