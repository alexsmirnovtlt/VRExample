// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Structs/MultiplayerStructs.h"
#include "Gameplay/Player/Controllers/VRMultiplayerPlayerController.h"
#include "VRFullBodyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VRFULLBODY_API AVRFullBodyPlayerController : public AVRMultiplayerPlayerController
{
	GENERATED_BODY()

	AVRFullBodyPlayerController();

	friend class AFullBodyGameMode;

public:
	virtual void Destroyed() override;
protected:
	virtual void OnPossess(APawn* aPawn) override;

public:
	// Offline local player only. NewAvatarData does not persist so must be saved manually
	UFUNCTION(BlueprintCallable)
	void HotSwapAvatarData(FVRFullBodyAvatarData NewAvatarData);
	UFUNCTION(BlueprintCallable)
	void RespawnAvatar() { HotSwapAvatarData(AvatarData); };
	const FVRFullBodyAvatarData& GetAvatarData() const { return AvatarData; };
protected:
	UPROPERTY(BlueprintReadOnly)
	bool IsFullBodyAvatar;
	UPROPERTY(BlueprintReadOnly)
	FVRFullBodyAvatarData AvatarData;

	void SpawnAvatar(APawn* aPawn);

	AFullBodyAvatar* AuthAvatarActor;
};