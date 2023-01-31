// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Controllers/VRMultiplayerPlayerController.h"

#include "Gameplay/CoreManagers/GameInstances/MultiplayerGameInstance.h"
#include "Gameplay/Interfaces/GameInstance/LoadingScreenProvider.h"
#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"
#include "Gameplay/Settings/VRBaseMultiplayerSettings.h"

AVRMultiplayerPlayerController::AVRMultiplayerPlayerController()
{
	bIsLocalPlayerTalking = false;
	bFadeCameraOnStart = false;
	FadeCameraTime = 1.5f;
}

void AVRMultiplayerPlayerController::ChangeLocalTalkingState(bool& bIsTalking)
{
	bIsLocalPlayerTalking = !bIsLocalPlayerTalking;
	ToggleSpeaking(bIsLocalPlayerTalking);
	bIsTalking = bIsLocalPlayerTalking;
}

void AVRMultiplayerPlayerController::ReceiveNotification_NoParams_Implementation()
{
	// Hiding blackscreen if able and then starting to fade main camera
	if (const auto GameSettings = GetMutableDefault<UVRBaseMultiplayerSettings>())
	{
		if(GameSettings && GameSettings->bShowLoadingScreenOnLevelChange)
			RemoveBlackScreenAfterLevelTransition();
	}
	if (bFadeCameraOnStart) FadeCameraToBlack(FadeCameraTime);
}

void AVRMultiplayerPlayerController::RemoveBlackScreenAfterLevelTransition()
{
	auto GameInstance = GetGameInstance();
	if (GameInstance && GameInstance->Implements<ULoadingScreenProvider>())
		ILoadingScreenProvider::Execute_LevelLoadingScreen_Hide(GameInstance);
}

void AVRMultiplayerPlayerController::FadeCameraToBlack(float Time)
{
	if (!ensure(PlayerCameraManager)) return;
	PlayerCameraManager->StartCameraFade(1.f, 0.f, FadeCameraTime, FLinearColor::Black, false, true);
}