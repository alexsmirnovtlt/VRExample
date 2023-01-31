// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/CoreManagers/GameModes/VRAndNonVRGameModeBase.h"
#include "FullBodyGameMode.generated.h"

class AVRFullBodyPawn;
class AVRFullBodyPlayerController;

/**
 * Class that is able to spawn multiple different types of pawns: NonVR, Regular VR (HMD+2Controllers), FullBodyVR OpenXR and FullbodyVR with Vive Trackers
 */
UCLASS()
class VRFULLBODY_API AFullBodyGameMode : public AVRAndNonVRGameModeBase
{
	GENERATED_BODY()

public:
	AFullBodyGameMode();
	virtual APlayerController* Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
	virtual UClass* GetDefaultPawnClassForController_Implementation(class AController* InController) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Parameters")
	FName FullBodyOptionString;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Parameters")
	FName ViveTrackersOptionString;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Custom Parameters")
	FName AvatarDataOptsString;

	UPROPERTY(EditDefaultsOnly, Category = "Custom Parameters - FullBody")
	TSubclassOf<AVRFullBodyPawn> PawnClass_3DoF;
	UPROPERTY(EditDefaultsOnly, Category = "Custom Parameters - FullBody")
	TSubclassOf<AVRFullBodyPawn> PawnClass_6DoFOrMore;

	void ApplyPlayerAvatarDataFromSave(AVRFullBodyPlayerController* PC, bool bIsUsingTrackers = false);
	FORCEINLINE void ApplyPlayerFullbodyTags(AVRFullBodyPlayerController* PC, bool bWithTrackers);
};
