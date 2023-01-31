// 2021-2022 Alexander Smirnov


#include "Gameplay/CoreManagers/GameInstances/MultiplayerGameInstance.h"

#include "Widgets/SWeakWidget.h"

#include "Net/VoiceConfig.h"
#include "Online.h"

#include "Gameplay/Settings/VRBaseMultiplayerSettings.h"
#include "UI/Slate/VRLoadingScreenWidget.h"

void UMultiplayerGameInstance::Init()
{
	Super::Init();

	if (const auto GameSettings = GetMutableDefault<UVRBaseMultiplayerSettings>())
		bShowBlackScreenOnLevelChange = GameSettings->bShowLoadingScreenOnLevelChange;

	LastNetworkError = 0xFF;
	VRLoadingScreenWidget = nullptr;

	if (GEngine) GEngine->NetworkFailureEvent.AddUObject(this, &UMultiplayerGameInstance::OnNetworkFailureHappened);
}

void UMultiplayerGameInstance::OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld)
{
	Super::OnWorldChanged(OldWorld, NewWorld);

	// TODO Loading Screen doesnt work as intended with OpenXR, widget just covers one eye in the top left corner

	if (bShowBlackScreenOnLevelChange && !IsDedicatedServerInstance())
	{
		if (NewWorld && !NewWorld->GetFName().ToString().Equals("Untitled"))
			LevelLoadingScreen_Show_Implementation();
	}
}

#pragma region INetworkFailureProvider 

void UMultiplayerGameInstance::OnNetworkFailureHappened(UWorld* World, UNetDriver* Driver, ENetworkFailure::Type FailureType, const FString& ErrorStr)
{
	if ((uint8)FailureType != 3) return; // Only interested in the ConnectionLost

	LastNetworkError = (uint8)FailureType;
	LastNetworkErrorStr = ErrorStr;

	if (auto VoiceInterface = Online::GetVoiceInterface())
	{
		// Very specific crash handling, but might be useful or at least harmless otherwise.
		// While using Steam Voice with VOIPTalker, if currently talking server disconnects, clients will fail ensure about VoipListenerSynthComponent and then crash
		UVOIPStatics::ClearAllSettings();
		VoiceInterface->StopNetworkedVoice(0);
		VoiceInterface->UnregisterLocalTalkers();
		VoiceInterface->RemoveAllRemoteTalkers();
	}
}

ENetworkFailure::Type UMultiplayerGameInstance::GetAndResetLastClientConnectionError_Implementation()
{
	auto Error = (ENetworkFailure::Type)LastNetworkError;
	LastNetworkError = 0xFF;
	return Error;
}

FString UMultiplayerGameInstance::GetAndResetLastClientConnectionErrorString_Implementation()
{
	FString ErrorStr = LastNetworkErrorStr;
	LastNetworkErrorStr = "";
	return ErrorStr;
}

#pragma endregion

#pragma region ILoadingScreenProvider 

void UMultiplayerGameInstance::LevelLoadingScreen_Show_Implementation()
{
	if (!VRLoadingScreenWidget.IsValid()) VRLoadingScreenWidget = SNew(SVRLoadingScreenWidget);

	if (auto Viewport = GetGameViewportClient())
	{
		Viewport->AddViewportWidgetContent(
			SAssignNew(VRLoadingScreenWidgetContainer, SWeakWidget)
			.PossiblyNullContent(VRLoadingScreenWidget), 0
		);
	}
}

void UMultiplayerGameInstance::LevelLoadingScreen_Hide_Implementation()
{
	if (!VRLoadingScreenWidget.IsValid()) return;

	if (auto Viewport = GetGameViewportClient())
	{
		Viewport->RemoveViewportWidgetContent(VRLoadingScreenWidgetContainer.ToSharedRef());
		VRLoadingScreenWidget.Reset();
	}
}

#pragma endregion