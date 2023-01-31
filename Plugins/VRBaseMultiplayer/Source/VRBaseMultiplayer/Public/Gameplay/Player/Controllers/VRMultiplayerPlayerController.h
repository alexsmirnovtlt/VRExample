// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "General/Interfaces/Notifiable.h"

#include "VRMultiplayerPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API AVRMultiplayerPlayerController : public APlayerController, public INotifiable
{
	GENERATED_BODY()
	
public:
	AVRMultiplayerPlayerController();
	virtual void ClientEnableNetworkVoice_Implementation(bool bEnable) override {}; // Disable forced open mic

public:
	// This gets called from a pawn when it is fully ready for a local player
	virtual void ReceiveNotification_NoParams_Implementation();

	UFUNCTION(BlueprintCallable, Category = "Multiplayer Player Controller")
	void ChangeLocalTalkingState(bool& bIsTalking);
	bool IsLocalPlayerTalking() const { return bIsLocalPlayerTalking; }

protected:
	void RemoveBlackScreenAfterLevelTransition();
	void FadeCameraToBlack(float Time);

	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Camera Fade")
	bool bFadeCameraOnStart;
	UPROPERTY(EditDefaultsOnly, Category = "Customizable - Camera Fade")
	float FadeCameraTime;

	bool bIsLocalPlayerTalking;
};
