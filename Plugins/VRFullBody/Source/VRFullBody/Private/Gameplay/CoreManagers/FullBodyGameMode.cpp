// 2021-2022 Alexander Smirnov


#include "Gameplay/CoreManagers/FullBodyGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "Gameplay/CoreManagers/Subsystems/GameInstance/PersistentVRDataSubsystem.h"
#include "Gameplay/Player/Controller/VRFullBodyPlayerController.h"
#include "Settings/VRFullBodyDataSaveGame.h"
#include "VRFullBody.h"

AFullBodyGameMode::AFullBodyGameMode()
{
	bPauseable = false;

	FullBodyOptionString = TEXT("fullbody"); // both for CMD and Options use
	ViveTrackersOptionString = TEXT("trackers");

	AvatarDataOptsString = TEXT("avatar"); // for Options use only

	PlayerControllerClass = AVRFullBodyPlayerController::StaticClass();
}

APlayerController* AFullBodyGameMode::Login(UPlayer* NewPlayer, ENetRole InRemoteRole, const FString& Portal, const FString& Options, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	auto NewPlayerController = Super::Login(NewPlayer, InRemoteRole, Portal, Options, UniqueId, ErrorMessage);
	UE_LOG(VRFullbodyLog, Warning, TEXT("AFullBodyGameMode::Login New Player with options: %s"), *Options);
	if (NewPlayerController->ActorHasTag(VRModeOptionString)) // Tag was assigned on Super::Login() above
	{
		auto FullBodyPC = Cast<AVRFullBodyPlayerController>(NewPlayerController);
		if (!FullBodyPC) { ensure(FullBodyPC); return NewPlayerController; }

		bool bParseFullBodyOptions = true;
		bool bSetAvatarDataFromOptions = true;
		bool bUsingTrackers = false;

		if (InRemoteRole == ENetRole::ROLE_SimulatedProxy)
		{ // Check Command Line Args for local player first
			auto CmdString = FString(FCommandLine::Get());
			if (CmdString.Contains("-" + ViveTrackersOptionString.ToString())) // if ViveTrackersOptionString option is present then apply both fullbody and trackers options
			{
				bUsingTrackers = true;
				bParseFullBodyOptions = false;
				ApplyPlayerFullbodyTags(FullBodyPC, true);
			}
			else if (CmdString.Contains("-" + FullBodyOptionString.ToString()))
			{
				bParseFullBodyOptions = false;
				ApplyPlayerFullbodyTags(FullBodyPC, false);
			}
			// Looking up for saved data
			if (!bParseFullBodyOptions)
			{
				bSetAvatarDataFromOptions = false;
				ApplyPlayerAvatarDataFromSave(FullBodyPC, bUsingTrackers);
			}
			if (bParseFullBodyOptions && bSetAvatarDataFromOptions) // all previous option checks were inaffective
			{ // Special case for PIE when a level starts without any cmd or option params and we reload a level with params to indicate that the pawn should be of full body type
				bUsingTrackers = UGameplayStatics::HasOption(Options, ViveTrackersOptionString.ToString());
				bool bFullBodyOption = UGameplayStatics::HasOption(Options, FullBodyOptionString.ToString());
				if (bFullBodyOption || bUsingTrackers)
				{ 
					bSetAvatarDataFromOptions = false;
					ApplyPlayerAvatarDataFromSave(FullBodyPC, bUsingTrackers);
				};
			}
		}

		if (bParseFullBodyOptions)
		{
			if (UGameplayStatics::HasOption(Options, ViveTrackersOptionString.ToString()))  // if ViveTrackersOptionString option is present then apply both fullbody and trackers options
				ApplyPlayerFullbodyTags(FullBodyPC, true);
			else if (UGameplayStatics::HasOption(Options, FullBodyOptionString.ToString()))
				ApplyPlayerFullbodyTags(FullBodyPC, false);
		}

		// TODO Received AvatarData does not get validated anywhere!

		// Setting full body params to a PlayerController, so it can pass them to the Pawn on Possess (Not really to a pawn but rather to a pawn`s Avatar Actor)
		if (bSetAvatarDataFromOptions)
		{
			FString AvatarStr = UGameplayStatics::ParseOption(Options, AvatarDataOptsString.ToString());
			if (!AvatarStr.IsEmpty()) FullBodyPC->AvatarData.InitFromString(AvatarStr);
		}
	}
	return NewPlayerController;
}

UClass* AFullBodyGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	if (InController->ActorHasTag(VRModeOptionString))
	{
		if (InController->ActorHasTag(FullBodyOptionString))
		{
			if (InController->ActorHasTag(ViveTrackersOptionString)) return PawnClass_6DoFOrMore;
			else return PawnClass_3DoF;
		}
		else return PawnClass_VR;
	}
	return PawnClass_NonVR;
}

void AFullBodyGameMode::ApplyPlayerFullbodyTags(AVRFullBodyPlayerController* PC, bool bWithTrackers)
{
	PC->IsFullBodyAvatar = true;
	PC->Tags.Add(FullBodyOptionString);
	if (bWithTrackers) PC->Tags.Add(ViveTrackersOptionString);
}

void AFullBodyGameMode::ApplyPlayerAvatarDataFromSave(AVRFullBodyPlayerController* PC, bool bIsUsingTrackers)
{
	if (auto SaveGameSubsystem = GetGameInstance()->GetSubsystem<UPersistentVRDataSubsystem>())
		if (auto SavedData = SaveGameSubsystem->GetSaveGameData())
			PC->AvatarData = SavedData->AvatarData;
	PC->AvatarData.IsUsingAdditionalTrackers = bIsUsingTrackers;
}