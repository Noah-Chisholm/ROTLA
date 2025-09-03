#include "DebugProjectile.h"
#include "Math/UnrealMathUtility.h"

ADebugProjectile::ADebugProjectile() {
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
}

void ADebugProjectile::BeginPlay() {
	Super::BeginPlay();
}

void ADebugProjectile::Initilize(FProjectileData Data) {
	SpawningWeapon = Data;
	Fire();
}

void ADebugProjectile::Fire() {
	FVector AccuracyUnitVector = FMath::VRandCone(GetActorForwardVector(), FMath::DegreesToRadians(SpawningWeapon.accuracy));
	FVector EndingLocation = GetActorLocation() + AccuracyUnitVector * SpawningWeapon.range;
	FCollisionQueryParams ColParams;
	ColParams.AddIgnoredActor(this);
	auto* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	ColParams.AddIgnoredActor(Player);
	TArray<AActor*> AttachedToPlayer;
	Player->GetAttachedActors(AttachedToPlayer);
	ColParams.AddIgnoredActors(AttachedToPlayer);
	FHitResult Hit;
	DrawDebugLine(GetWorld(), GetActorLocation(), EndingLocation, FColor::Red, true);
	GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), EndingLocation, ECollisionChannel::ECC_Camera, ColParams);
	if (Hit.bBlockingHit && Hit.GetActor()) {
		auto* Killable = Cast<IDamageInterface>(Hit.GetActor());
		if(Killable)
			Killable->DealDamage(SpawningWeapon.Damage);
	}
	
	this->Destroy();
}