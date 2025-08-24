#include "DebugWeapon.h"
#include "DebugProjectile.h"
#include "Components/ArrowComponent.h"
#include "ROTLA/Utility/GI_ROTLA/GI_ROTLA.h"
#include "Components/SceneComponent.h"

ADebugWeapon::ADebugWeapon() {
	PrimararyGripLocation = CreateDefaultSubobject<USceneComponent>("PrimaryGripLocation");
	SetRootComponent(PrimararyGripLocation);
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>("Skeleton");
	Mesh->SetupAttachment(PrimararyGripLocation);
	PrimaryFireLocation = CreateDefaultSubobject<UArrowComponent>("PrimaryFireLocation");
	PrimaryFireLocation->ComponentTags.Add("FireLocation");
	PrimaryFireLocation->ComponentTags.Add("Primary");
	PrimaryFireLocation->SetupAttachment(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	Mesh->SetCollisionProfileName(FName("BlockAll"));
	PrimaryActorTick.bCanEverTick = true;
	WeaponFlag = NewObject<UFlag>();
	SetFlag(EWeaponFlags::IsCooledDown);
}

void ADebugWeapon::OverwriteData(FWeaponData NewData) {
	Data = NewData;

	// Supply a correct default if null
	if (Data.MeshToUse.IsNull())
	{
		Data.MeshToUse = TSoftObjectPtr<USkeletalMesh>(
			FSoftObjectPath(TEXT("/Game/Weapons/NullMesh/NullMesh.NullMesh")));
	}

	// Get current/desired as strings for a fair compare
	const FString CurrentMeshPath = (Mesh && Mesh->GetSkeletalMeshAsset())
		? Mesh->GetSkeletalMeshAsset()->GetPathName()
		: FString();

	const FString DesiredMeshPath = Data.MeshToUse.IsNull()
		? FString()
		: Data.MeshToUse.ToSoftObjectPath().ToString();

	if (CurrentMeshPath != DesiredMeshPath)
	{
		USkeletalMesh* NewMesh = Data.MeshToUse.LoadSynchronous();
		if (NewMesh)
		{
			Mesh->SetSkeletalMesh(NewMesh);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to LoadSynchronous SkeletalMesh at '%s'"), *DesiredMeshPath);
		}
	}
	FAttachmentTransformRules Rules(EAttachmentRule::SnapToTarget, true);
	PrimaryFireLocation->AttachToComponent(Mesh, Rules, "FLP");
	FVector FireLocationStart = Mesh->GetSocketLocation("FireLocation");
	FVector FireLocationEnd = Mesh->GetSocketLocation("FireLocation_end");
	FVector FireLocationDirection = FireLocationEnd - FireLocationStart;
	PrimaryFireLocation->SetWorldRotation(FireLocationDirection.Rotation());
	FVector MeshGripLocation = Mesh->GetSocketLocation(FName("GLP"));
	FVector ActualGripLocation = PrimararyGripLocation->GetComponentLocation();
	FVector Offset = MeshGripLocation - ActualGripLocation;
	Mesh->AddLocalOffset(-Offset);
	//PrimararyGripLocation->AttachToComponent(Mesh, Rules, "GLP");

	TArray FireLocations = GetComponentsByTag(USceneComponent::StaticClass(), "FireLocation");
	for (int i = 0; i < FireLocations.Num(); i++) {
		if (!FireLocations[i]->ComponentHasTag(FName("Primary")))
			FireLocations[i]->DestroyComponent();
	}

	auto Sockets = Mesh->GetAllSocketNames();
	TArray<FName> FireSockets;
	for (int i = 0; i < Sockets.Num(); i++) {
		if (Sockets[i].ToString().Contains("FL") && Sockets[i].ToString() != "FLP") {
			FireSockets.Add(Sockets[i]);
		}
	}
	for (int i = 0; i < FireSockets.Num(); i++) {
		auto* temp = NewObject<USceneComponent>(this, "FireLocation_" + i);
		temp->ComponentTags.Add("FireLocation");
		temp->AttachToComponent(Mesh, Rules, FireSockets[i]);
	}

	Ammo = Data.StartingAmmo;
}

void ADebugWeapon::BeginPlay() {
	Super::BeginPlay();
	Data = FWeaponData();
	SetActorTickEnabled(true);
}

bool ADebugWeapon::StartPrimaryInteraction()
{
	SetFlag(EWeaponFlags::WantsToFire);
	SetFlag(EWeaponFlags::IsCharging);
	return true;
}

bool ADebugWeapon::FinishPrimaryInteraction()
{
	RemoveFlag(EWeaponFlags::IsCharging);
	RemoveFlag(EWeaponFlags::IsCharged);
	RemoveFlag(EWeaponFlags::WantsToFire);
	RemoveFlag(EWeaponFlags::Fired);
	return true;
}

void ADebugWeapon::Fire() {
	TArray FireLocations = GetComponentsByTag(USceneComponent::StaticClass(), "FireLocation");
	int32 Counter = 0;
	for (int32 i = 0; i < Data.ProjectileCount; i++) {
		FActorSpawnParameters SpawnParams;
		UClass* ProjectileToSpawn = ADebugProjectile::StaticClass();
		switch (Data.Projectile)
		{
		case EProjectileType::ROCKET:
			//Not Implemented
			break;
		case EProjectileType::HOMINGROCKET:
			//Not Implemented
			break;
		case EProjectileType::HOMINGBULLET:
			//Not Implemented
			break;
		case EProjectileType::BEAM:
			//Not Implemented
			break;
		case EProjectileType::PLASMABOLT:
			//Not Implemented
			break;
		case EProjectileType::ARROW:
			//Not Implemented
			break;
		default:
			break;
		}
		auto* FireLocation = Cast<USceneComponent>(FireLocations[Counter++]);
		if (Counter >= FireLocations.Num())
			Counter = 0;
		FProjectileData NewData;
		NewData.accuracy = Data.accuracy;
		NewData.Damage = Data.Damage;
		GetWorld()->SpawnActor<ADebugProjectile>(ProjectileToSpawn, FTransform(FireLocation->GetComponentRotation(), FireLocation->GetComponentLocation(), FireLocation->GetComponentScale()))->Initilize(NewData);
	}
	Ammo--;
}

bool ADebugWeapon::SecondaryInteraction()
{
	return false;
}

void* ADebugWeapon::InteractableInteraction(void* DataIn, bool& Success)
{
	return this;
}

void ADebugWeapon::Tick(float DT) {
	Super::Tick(DT);
	if (HasFlag(EWeaponFlags::IsCharging) && ChargeAmount < Data.ChargeTime) {
		ChargeAmount += DT;
		if (ChargeAmount >= Data.ChargeTime) {
			SetFlag(EWeaponFlags::IsCharged);
			ChargeAmount = Data.ChargeTime;
		}
	}
	if (!HasFlag(EWeaponFlags::IsCharging) && ChargeAmount > 0) {
		ChargeAmount -= DT;
		if (ChargeAmount < 0) {
			ChargeAmount = 0;
		}
	}
	if (HasFlag(EWeaponFlags::IsCoolingDown)) {
		CooledAmount += DT;
		if (CooledAmount >= Data.ShotCoolDown) {
			SetFlag(EWeaponFlags::IsCooledDown);
			RemoveFlag(EWeaponFlags::IsCoolingDown);
			CooledAmount = 0;
		}
	}
	if (!HasFlag(EWeaponFlags::WantsToFire) || HasFlag(EWeaponFlags::IsReloading) || !HasFlag(EWeaponFlags::IsCooledDown) || !HasFlag(EWeaponFlags::IsCharged) || Ammo <= 0)
		return;
	switch (Data.Pattern)
	{
	case EFirePattern::SINGLE:
		ProcessSingleShot();
		break;
	case EFirePattern::FULLAUTO:
		ProcessAutoShot();
		break;
	case EFirePattern::BURST:
		ProcessBurstShot();
		break;
	default:
		break;
	}
	SetFlag(EWeaponFlags::IsCoolingDown);
	RemoveFlag(EWeaponFlags::IsCooledDown);
}

void ADebugWeapon::ProcessSingleShot() {
	if (!HasFlag(EWeaponFlags::Fired)) {
		SetFlag(EWeaponFlags::Fired);
		ChargeAmount = 0;
		RemoveFlag(EWeaponFlags::IsCharged);
		RemoveFlag(EWeaponFlags::IsCharging);
		Fire();
	}
}

void ADebugWeapon::ProcessAutoShot() {
	Fire();
}

void ADebugWeapon::ProcessBurstShot() {
	auto& Timer = GetWorld()->GetTimerManager();
	int ShotsToShoot = Data.BurstShots;
	int AmmoLeft = Ammo - ShotsToShoot;
	if (AmmoLeft < 0)
		ShotsToShoot += AmmoLeft;
	for (int32 i = 1; i <= ShotsToShoot; i++) {
		FTimerHandle TempTimer;
		Timer.SetTimer(TempTimer, this, &ADebugWeapon::Fire, Data.BurstDelay * i);
	}
}

uint32 ADebugWeapon::ReloadAmmo(uint32 AmmountToReload) {
	SetFlag(EWeaponFlags::IsReloading);
	RemoveFlag(EWeaponFlags::IsCharged);
	RemoveFlag(EWeaponFlags::IsCharging);
	ChargeAmount = 0;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &ADebugWeapon::EndReload, Data.ReloadTime);
	uint32 AmountToReturn = Ammo;
	uint32 ClipSize = Data.ClipSize;
	if (AmmountToReload >= ClipSize) {
		Ammo = ClipSize;
		AmountToReturn += AmmountToReload - ClipSize;
	}
	else {
		Ammo += AmmountToReload;
		if (Ammo > ClipSize)
			AmountToReturn = Ammo - ClipSize;
		Ammo = ClipSize;
	}
	return AmountToReturn;
}

FString ADebugWeapon::GetAmmoString()
{
	if (HasFlag(EWeaponFlags::IsReloading))
		return FString::Printf(TEXT("- / %d"), Data.ClipSize);
	return FString::Printf(TEXT("%d / %d"), Ammo, Data.ClipSize);
}

void ADebugWeapon::EndReload() {
	RemoveFlag(EWeaponFlags::IsReloading);
	if (!HasFlag(EWeaponFlags::WantsToFire))
		SetFlag(EWeaponFlags::IsCharging);
}