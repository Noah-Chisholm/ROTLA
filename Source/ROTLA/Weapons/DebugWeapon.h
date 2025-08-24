#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ROTLA/Utility/Interfaces/InteractableInterface/InteractableInterface.h"
#include "ROTLA/Utility/Flag.h"
#include "WeaponData.h"
#include "DebugWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponFlags : uint8
{
	IsCharged,
	IsCharging,
	IsCooledDown,
	IsCoolingDown,
	IsReloading,
	Fired,
	WantsToFire
};

UCLASS()
class ADebugWeapon : public AActor, public IInteractableInterface
{
	GENERATED_BODY()

public: 
	ADebugWeapon();
	void OverwriteData(FWeaponData NewData);

	virtual void BeginPlay() override;
	virtual bool StartPrimaryInteraction() override;
	virtual bool FinishPrimaryInteraction() override;
	virtual bool SecondaryInteraction() override;
	virtual void* InteractableInteraction(void* Data, bool& Success) override;
	virtual void Tick(float DeltaTime) override;

	uint32 ReloadAmmo(uint32 AmmountToReload);
	FString GetAmmoString();

	UFlag* WeaponFlag;

	UFUNCTION(BlueprintCallable)
	void SetFlag(EWeaponFlags Value) {
		WeaponFlag->_SetFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	void RemoveFlag(EWeaponFlags Value) {
		WeaponFlag->_RemoveFlag(static_cast<int32>(Value));
	}; 
	UFUNCTION(BlueprintCallable)
	bool HasFlag(EWeaponFlags Value) const {
		return WeaponFlag->_HasFlag(static_cast<int32>(Value));
	};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class UArrowComponent* PrimaryFireLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	class USceneComponent* PrimararyGripLocation;

	friend class UDebugCommandHandler;

protected:
	UPROPERTY(BlueprintReadWrite)
	FWeaponData Data;
private:
	uint32 Ammo = 0;

	float ChargeAmount;
	float CooledAmount;
	FTimerHandle ReloadTimer;

	void EndReload();
	void Fire();
	void ProcessSingleShot();
	void ProcessAutoShot();
	void ProcessBurstShot();
};