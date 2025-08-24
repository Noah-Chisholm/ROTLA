#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamageInterface.h"
#include "DebugProjectile.generated.h"

USTRUCT()
struct FProjectileData
{
	GENERATED_BODY()

	float accuracy = 0.0f; //The degrees of a cone that will decide trajectory
	float range = 10000.0f; //The Distance a projectile may travel before despawning
	FDamage Damage = FDamage(EDamageTypes::Normal, 10.0f);
	float HomingDuration = -1.0f; // Length of time that homing is active
	float HomingAccuracy = -1.0f; // The radius of a circle around the targeted entity that a homing projectile may strike
};

UCLASS()
class ADebugProjectile : public AActor
{
	GENERATED_BODY()

public: 
	ADebugProjectile();

	virtual void BeginPlay() override;

	void Initilize(FProjectileData Data);

	virtual void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* Root;

protected:
private:
	FProjectileData SpawningWeapon;
};