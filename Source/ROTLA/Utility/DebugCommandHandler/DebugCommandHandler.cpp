#include "DebugCommandHandler.h"
#include "ROTLA/Player/PlayerPawn.h"
#include "ROTLA/Weapons/DebugWeapon.h"
#include "Camera/CameraComponent.h"
#include "ROTLA/Enemies/EnemyRegistry.h"
#include "ROTLA/Weapons/WeaponRegistry.h"
#include "ROTLA/Weapons/DebugProjectile.h"

#define NEWCOMMAND(NAME) bool UDebugCommandHandler::NAME(FJsonSerializableArray Data, FString& ErrorMessage, bool IsFinished)

#define PARSEFLOAT(VAR, STRING)                    \
	if (!LexTryParseString<float>(VAR, *(STRING)))                   \
	{                                                                \
		if (IsFinished)                                              \
			ErrorMessage = (STRING) + Invalid + FloatEx;             \
		else                                                         \
			ErrorMessage = FloatEx;                                  \
		return false;                                                     \
	}

#define PARSEUINT(VAR, STRING)                    \
	if (!LexTryParseString<uint32>(VAR, *(STRING)))                   \
	{                                                                \
		if (IsFinished)                                              \
			ErrorMessage = (STRING) + Invalid + UIntEx;             \
		else                                                         \
			ErrorMessage = UIntEx;                                  \
		return false;                                                     \
	}

UDebugCommandHandler::UDebugCommandHandler()
{
}

bool UDebugCommandHandler::ProcessCommand(const FString& Command, FString& ErrorMessage, bool IsFinished)
{
	TArray<FString> CommandComponentList;
	auto CommandArray = Command.GetCharArray();
	FString CurString;
	for (int32 i = 0; i < CommandArray.Num(); i++) {
		if (CommandArray[i] != ' ') {
			CurString.AppendChar(CommandArray[i]);
		}
		else {
			CommandComponentList.Add(CurString);
			CurString = "";
		}
	}
	if(CurString != "")
		CommandComponentList.Add(CurString);
	if (CommandComponentList.Num() < 1)
		return false;
	FString CommandName = CommandComponentList[0];
	if (CommandList.Find(FName(CommandName)) != INDEX_NONE) {
		if (CommandName == CommandList[0])
			return SetPlayerWeapon(CommandComponentList, ErrorMessage, IsFinished);
		if (CommandName == CommandList[1])
			return GivePlayerWeapon(CommandComponentList, ErrorMessage, IsFinished);
		if (CommandName == CommandList[2])
			return SummonEnemy(CommandComponentList, ErrorMessage, IsFinished);
	}
	else {
		if(IsFinished) 
			ErrorMessage = "Cannot find command: " + CommandName;
		else {
			for (int32 i = 0; i < CommandList.Num(); i++)
			{
				if (CommandList[i].ToString().Contains(CommandName))
					ErrorMessage.Append(CommandList[i].ToString());
				if (!(i == CommandList.Num() - 1))
					ErrorMessage.AppendChar(',');
			}
		}
		return false;
	}
	return false;
}

void UDebugCommandHandler::Init(UGI_ROTLA* GI) {
	GameInstance = GI;
}

bool UDebugCommandHandler::SetPlayerWeapon(FJsonSerializableArray Data, FString& ErrorMessage, bool IsFinished) {
	//TODO: Pull Class from command, and check it, and provide auto-completion
	ADebugWeapon* NewWeapon = GameInstance->GetWorld()->SpawnActor<ADebugWeapon>(ADebugWeapon::StaticClass(), FTransform());
	auto* Player = Cast<APC_ROTLA>(GameInstance->GetFirstLocalPlayerController()->GetPawn());
	Player->EquipedWeapon = NewWeapon;
	FAttachmentTransformRules Rules = FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true);
	NewWeapon->PrimararyGripLocation->AttachToComponent(Cast<APC_ROTLA>(GameInstance->GetFirstLocalPlayerController()->GetPawn())->PrimaryGripLocation, Rules);
	float Range;
	PARSEFLOAT(Range, Data[1]);
	uint32 ProjectileCount;
	PARSEUINT(ProjectileCount, Data[2]);
	const UEnum* EnumPtr = StaticEnum<EDamageTypes>();
	int64 Enumval = EnumPtr->GetValueByName(*Data[3]);
	if (Enumval == INDEX_NONE) {
		if (IsFinished)
			ErrorMessage = Data[3] + Invalid + "Damage Type!";
		else {
			for (int32 i = 0; i < EnumPtr->NumEnums() - 1; i++) {
				ErrorMessage.Append(EnumPtr->GetNameStringByIndex(i));
				if (i != EnumPtr->NumEnums() - 2)
					ErrorMessage.AppendChar(',');
			}
		}
		return false;
	}
	EDamageTypes TypeIn = static_cast<EDamageTypes>(Enumval);
	float AmountIn;
	PARSEFLOAT(AmountIn, Data[4])
	FDamage Damage = FDamage(TypeIn, AmountIn);
	float Accuracy;
	PARSEFLOAT(Accuracy, Data[5])
	FString ClassName = Data[6];
	FString ClassPath = FString::Printf(TEXT("/Game/Blueprints/%s.%s_C"), *ClassName, *ClassName);
	TSubclassOf<ADebugProjectile> ProjectileType = StaticLoadClass(ADebugProjectile::StaticClass(), nullptr, *ClassPath);
	FWeaponData NewData; //FWeaponData(Range, ProjectileCount, Damage, Accuracy, ProjectileType);
	NewWeapon->OverwriteData(NewData);
	return true;
}

NEWCOMMAND(GivePlayerWeapon) {
	if(Data.Num() < 2) {
		ErrorMessage = "Please provide a valid weapon ID: ";
		auto Options = GameInstance->Weapons->GetEntries();
		for (int i = 0; i < Options.Num(); i++) {
			ErrorMessage.Append(Options[i].ToString());
			if (i != Options.Num() - 1)
				ErrorMessage.Append(", ");
		}
		return false;
	}
	if (!GameInstance->Weapons->IsKey(FName(Data[1]))) {
		if (IsFinished)
			ErrorMessage = Data[1] + Invalid + " Weapon ID :(";
		else {
			auto Options = GameInstance->Weapons->GetEntries();
			for (int i = 0; i < Options.Num(); i++) {
				ErrorMessage.Append(Options[i].ToString());
				if (i != Options.Num() - 1)
					ErrorMessage.Append(", ");
			}
			return false;
		}
	}
	FWeaponData NewData;
	GameInstance->Weapons->GetFromName(FName(Data[1]), NewData);
	FVector Location;
	if (Data.Num() == 2) {
		Location = GameInstance->GetFirstGamePlayer()->GetPlayerController(GameInstance->GetWorld())->GetPawn()->GetActorLocation();
	}
	else {
		if (Data.Num() < 5) {
			ErrorMessage = "Not enough arguments to call function! Please provide a location as: " + FloatEx + " " + FloatEx + " " + FloatEx;
			return false;
		}
		else {
			float X, Y, Z;
			PARSEFLOAT(X, Data[2])
				PARSEFLOAT(Y, Data[3])
				PARSEFLOAT(Z, Data[4])
				Location = { X, Y, Z };
		}
	}
	if (!IsFinished)
		return false;
	ADebugWeapon* NewWeapon = GameInstance->GetWorld()->SpawnActor<ADebugWeapon>(ADebugWeapon::StaticClass(), FTransform(Location));
	NewWeapon->OverwriteData(NewData);
	return true;
}

NEWCOMMAND(SummonEnemy) {
	if (Data.Num() < 2) {
		ErrorMessage = "Please provide a valid Enemy ID: ";
		auto Options = GameInstance->Enemies->GetEntries();
		for (int i = 0; i < Options.Num(); i++) {
			ErrorMessage.Append(Options[i].ToString());
			if (i != Options.Num() - 1)
				ErrorMessage.Append(", ");
		}
		return false;
	}
	if (!GameInstance->Enemies->IsKey(FName(Data[1]))) {
		if (IsFinished)
			ErrorMessage = Data[1] + Invalid + " Enemy ID :(";
		else {
			auto Options = GameInstance->Enemies->GetEntries();
			for (int i = 0; i < Options.Num(); i++) {
				if (Options[i].ToString().Contains(Data[1])) {
					ErrorMessage.Append(Options[i].ToString());
					if (i != Options.Num() - 1)
						ErrorMessage.Append(", ");
				}
			}
			return false;
		}
	}
	TSoftClassPtr<ADebugEnemy> EnemyData;
	GameInstance->Enemies->GetFromName(FName(Data[1]), EnemyData);
	FVector Location;
	if (Data.Num() == 2) {
		auto* Player = Cast<APC_ROTLA>(GameInstance->GetFirstLocalPlayerController()->GetPawn());
		FVector FaceDirection = Player->FirstPersonCamera->GetForwardVector();
		FVector StartLocation = Player->FirstPersonCamera->GetComponentLocation();
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, StartLocation + FaceDirection * DOUBLE_BIG_NUMBER, ECC_Camera);
		if (Hit.bBlockingHit) {
			Location = Hit.ImpactPoint;
		}
		else {
			Location = Player->GetActorLocation() + FaceDirection * 999;
		}
	}
	else {
		ErrorMessage = "Too many arguments, there can only be one!";
		return false;
	}

	if (!IsFinished)
		return false;
	if (UClass* EnemyClass = EnemyData.LoadSynchronous())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ADebugEnemy* NewEnemy = GameInstance->GetWorld()->SpawnActor<ADebugEnemy>(
			EnemyClass,
			FTransform(Location),
			Params
		);
	}
	else {
		GameInstance->PrintDebugC(FText::FromString("Enemy: " + EnemyData.GetAssetName() + " could not be loaded correctly! It is likely that the enemy not packaged correctly."), MessageTypes::Error);;
	}
	return true;
}