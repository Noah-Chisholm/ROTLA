#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DamageInterface.generated.h"

UENUM(BlueprintType)
enum class EDamageTypes : uint8
{
	Normal,
	Burning,
	Corrosion
};

USTRUCT(BlueprintType)
struct FDamage
{
	GENERATED_BODY()

public:
	FDamage(EDamageTypes TypeIn, float AmountIn) : Type(TypeIn), Amount(AmountIn) {};
	FDamage() {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDamageTypes Type = EDamageTypes::Normal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Amount = 0.0f;
};

UINTERFACE(BlueprintType)
class UDamageInterface : public UInterface
{
	GENERATED_BODY()
};

class IDamageInterface
{
	GENERATED_BODY()

public:
	virtual bool DealDamage(FDamage DamageToDeal);
};