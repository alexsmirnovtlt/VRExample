// 2021-2022 Alexander Smirnov


#include "Gameplay/CoreManagers/GameModes/VRAndNonVRGameModeBase.h"

#include "Kismet/GameplayStatics.h"
#include "Misc/CommandLine.h"

#if WITH_EDITOR
#include "Editor/UnrealEd/Classes/Editor/EditorEngine.h"
#endif

#include "Gameplay/Player/Controllers/VRMultiplayerPlayerController.h"

AVRAndNonVRGameModeBase::AVRAndNonVRGameModeBase()
{
	bPauseable = false;

	bDefaultPawnIsVR = false;
	VRModeOptionString = TEXT("vr");

	PlayerControllerClass = AVRMultiplayerPlayerController::StaticClass();
}

APlayerController* AVRAndNonVRGameModeBase::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	auto NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	if (!ensure(NewPlayerController)) return nullptr;

	if (ShouldSpawnPlayerAsVR(Options, InRemoteRole)) NewPlayerController->Tags.Add(VRModeOptionString);

	return NewPlayerController;
}

UClass* AVRAndNonVRGameModeBase::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->ActorHasTag(VRModeOptionString)) return PawnClass_VR;
	return PawnClass_NonVR;
}

bool AVRAndNonVRGameModeBase::ShouldSpawnPlayerAsVR(const FString& Options, ENetRole InRemoteRole) const
{
#if WITH_EDITOR
	if (GIsEditor) // Editor build will pick a pawn depending on a play mode (VR or regular)
	{
		if (UEditorEngine* EdEngine = Cast<UEditorEngine>(GEngine))
		{
			TOptional<FPlayInEditorSessionInfo> PlayInfo = EdEngine->GetPlayInEditorSessionInfo();
			if (PlayInfo.IsSet())
			{
				if (PlayInfo->OriginalRequestParams.SessionPreviewTypeOverride == EPlaySessionPreviewType::VRPreview) return true;
				else return bDefaultPawnIsVR;
			}
		}
	}
#endif

	if (bDefaultPawnIsVR) return true; // Ignore everything and just pick the VR pawn
	if (UGameplayStatics::HasOption(Options, VRModeOptionString.ToString())) return true; // Remote player spawns in VR if respective parameter was provided
	if (InRemoteRole == ENetRole::ROLE_SimulatedProxy) // Local Controller 
	{
		if (FString(FCommandLine::Get()).Contains("-"+VRModeOptionString.ToString())) return true;
	}

	return false;
}