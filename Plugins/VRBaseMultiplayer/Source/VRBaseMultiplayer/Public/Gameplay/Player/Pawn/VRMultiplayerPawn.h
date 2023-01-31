// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Pawns/MovableCamera/VRPawnWithMovableCam.h"
#include "Gameplay/Interfaces/VoipTalkerStatusReceiver.h"
#include "VRMultiplayerPawn.generated.h"

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API AVRMultiplayerPawn : public AVRPawnWithMovableCam, public IVoipTalkerStatusReceiver
{
	GENERATED_BODY()
public:
	AVRMultiplayerPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
public:
	UFUNCTION(BlueprintCallable, Category = "VR Multiplayer Pawn")
	AVRMultiplayerPlayerController* GetMultiplayerPlayerController();

protected:

	UPROPERTY(EditDefaultsOnly, Category = "VOIP")
	USoundAttenuation* VOIPAttenuation;

	virtual void OnPawnReadyToPlay_Internal(bool bLocalPlayer) override;

	UPROPERTY(VisibleAnywhere)
	class UMultiplayerVOIPTalker* VOIPTalker;

	UFUNCTION(BlueprintCallable, Category = "VR Multiplayer Pawn")
	void LeaveOnlineSession();
	void OnOnlineSessionDestroyed(bool bSuccess);
	UFUNCTION(BlueprintImplementableEvent, Category = "VR Multiplayer Pawn")
	void OnOnlineSessionClosed();
	FDelegateHandle DestroySessionHandle;

	UFUNCTION(BlueprintCallable, Category = "VR Multiplayer Pawn")
	void CleanupVoiceData();
};
