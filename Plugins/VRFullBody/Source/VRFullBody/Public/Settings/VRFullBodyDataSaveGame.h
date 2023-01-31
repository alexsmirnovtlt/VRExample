// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "General/Structs/MultiplayerStructs.h"
#include "VRFullBodyDataSaveGame.generated.h"

USTRUCT(BlueprintType)
struct VRFULLBODY_API FCalibratedBoneData
{
	GENERATED_BODY()

	FCalibratedBoneData()
	{
		LocationOffset = FVector();
		RotationOffset = FRotator();
	}

	UPROPERTY(Transient)
	FName BoneName;
	UPROPERTY(Transient)
	FVector LocationOffset;
	UPROPERTY(Transient)
	FRotator RotationOffset;
};

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRFullBodyDataSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVRFullBodyAvatarData AvatarData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform RightHandOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform LeftHandOffset;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Save Game")
	static const FString& GetAvatarDataSaveSlotName() { return SlotName; }
	static const FString SlotName;
};
