// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Pawn/VRMultiplayerPawn.h"

#include "Online.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Controllers/VRMultiplayerPlayerController.h"
#include "Gameplay/Player/ActorComponents/MultiplayerVOIPTalker.h"
#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"

AVRMultiplayerPawn::AVRMultiplayerPawn(const FObjectInitializer& ObjectInitializer)
{
	bReplicates = true;

	VOIPTalker = CreateDefaultSubobject<UMultiplayerVOIPTalker>(TEXT("VOIPTalker"));
}

void AVRMultiplayerPawn::OnPawnReadyToPlay_Internal(bool bLocalPlayer)
{
	if (!bLocalPlayer)
	{
		if (GetNetMode() != ENetMode::NM_Standalone)
		{
			VOIPTalker->Settings.ComponentToAttachTo = MainCamera;
			VOIPTalker->Settings.AttenuationSettings = VOIPAttenuation;

			VOIPTalker->RegisterWithPlayerState(GetPlayerState());
		}
	}

	Super::OnPawnReadyToPlay_Internal(bLocalPlayer);
}

AVRMultiplayerPlayerController* AVRMultiplayerPawn::GetMultiplayerPlayerController()
{
	if (auto PController = GetController())
	{
		return Cast<AVRMultiplayerPlayerController>(PController);
	}
	return nullptr;
}

void AVRMultiplayerPawn::LeaveOnlineSession()
{
	if (GetNetMode() == ENetMode::NM_Standalone) return;

	if (auto OnlineSessionSubsys = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>())
	{
		DestroySessionHandle = OnlineSessionSubsys->OnOnlineSessionDestroyed_Event.AddUObject(this, &AVRMultiplayerPawn::OnOnlineSessionDestroyed);
		OnlineSessionSubsys->DestroySession();
	}
}

void AVRMultiplayerPawn::OnOnlineSessionDestroyed(bool bSuccess)
{
	if (auto OnlineSessionSubsys = GetGameInstance()->GetSubsystem<UOnlineSessionSubsystem>())
	{
		OnlineSessionSubsys->OnOnlineSessionDestroyed_Event.Remove(DestroySessionHandle);
		DestroySessionHandle.Reset();
		if (bSuccess) OnOnlineSessionClosed();
	}
}

void AVRMultiplayerPawn::CleanupVoiceData()
{
	if (GetNetMode() == ENetMode::NM_Standalone) return;

	auto VoiceInterface = Online::GetVoiceInterface();
	if (!VoiceInterface) return;

	auto PC = GetMultiplayerPlayerController();
	bool IsLocalPlayer = PC ? PC->IsLocalPlayerController() : false;
	if (IsLocalPlayer)
	{
		UVOIPStatics::ClearAllSettings();
		VoiceInterface->StopNetworkedVoice(0);
		VoiceInterface->UnregisterLocalTalkers();
		VoiceInterface->RemoveAllRemoteTalkers();
	}
	else if (auto PState = GetPlayerState())
	{
		const FUniqueNetIdRepl& UniqueNetIdRepl = PState->GetUniqueId();
		if (UniqueNetIdRepl.IsValid())
		{
			UVOIPStatics::ResetPlayerVoiceTalker(UniqueNetIdRepl);
			VoiceInterface->UnregisterRemoteTalker(*UniqueNetIdRepl.GetUniqueNetId().Get());
		}
	}
}