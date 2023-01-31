// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystemUtils.h"

#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerController.h"

#include "Gameplay/Settings/VRBaseMultiplayerSettings.h"

FName const UOnlineSessionSubsystem::CustomSessionQueryKey_SessionName = TEXT("SESSION_NAME");

DEFINE_LOG_CATEGORY(OnlineSessionSubsystem);

bool UOnlineSessionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (const auto GameSettings = GetMutableDefault<UVRBaseMultiplayerSettings>())
		return GameSettings->bCreateMainOnlineSessionSubsystem;
	return false;
}

void UOnlineSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	CreateSessionCompleteDelegate = CreateSessionCompleteDelegate.CreateUObject(this, &UOnlineSessionSubsystem::OnOnlineSession_Created);
	StartSessionCompleteDelegate = StartSessionCompleteDelegate.CreateUObject(this, &UOnlineSessionSubsystem::OnOnlineSession_Started);
	DestroySessionCompleteDelegate = DestroySessionCompleteDelegate.CreateUObject(this, &UOnlineSessionSubsystem::OnOnlineSession_Destroyed);
	FindSessionsCompleteDelegate = FindSessionsCompleteDelegate.CreateUObject(this, &UOnlineSessionSubsystem::OnFindSessionsCompleted);
	JoinSessionCompleteDelegate = JoinSessionCompleteDelegate.CreateUObject(this, &UOnlineSessionSubsystem::OnJoinSessionCompleted);

	InitOnlineSessionSettings();
}

#pragma region Public_Functions

void UOnlineSessionSubsystem::CreateSession(int32 NumPublicConnections, const FString& CustomSessionName, const FString& MapName, const TArray<FOnlineSessionSettingsKeyPair> AdditionalSettings)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!SessionInterface) { OnOnlineSessionCreated_Event.Broadcast(false); return; }

	SessionSettings->NumPublicConnections = NumPublicConnections;

	OnlineSessionCreated_DelegateHandle = SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	SessionSettings->Set(SETTING_MAPNAME, MapName, EOnlineDataAdvertisementType::ViaOnlineService);
	SessionSettings->Set(CustomSessionQueryKey_SessionName, CustomSessionName, EOnlineDataAdvertisementType::ViaOnlineService);

	for (const auto& KeyValue : AdditionalSettings)
		SessionSettings->Set(KeyValue.Key, KeyValue.Value, (EOnlineDataAdvertisementType::Type) KeyValue.AdvertisementType);

	if (!SessionInterface->CreateSession(0, NAME_GameSession, *SessionSettings))
	{
		SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnlineSessionCreated_DelegateHandle);
		UE_LOG(OnlineSessionSubsystem, Warning, TEXT("Could not create a Session!"));
		OnOnlineSessionCreated_Event.Broadcast(false);
	}
}

void UOnlineSessionSubsystem::StartSession()
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) { OnOnlineSessionStarted_Event.Broadcast(false); return; }

	OnlineSessionStarted_DelegateHandle =
		SessionInterface->AddOnStartSessionCompleteDelegate_Handle(StartSessionCompleteDelegate);

	if (!SessionInterface->StartSession(NAME_GameSession))
	{
		SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnlineSessionStarted_DelegateHandle);
		UE_LOG(OnlineSessionSubsystem, Warning, TEXT("Could not start a Session!"));
		OnOnlineSessionStarted_Event.Broadcast(false);
	}
}

void UOnlineSessionSubsystem::DestroySession()
{
	if (SessionDestructionInProgress) { ensure(false); return; }

	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) { OnOnlineSessionDestroyed_Event.Broadcast(false); return; }
	
	SessionDestructionInProgress = true;
	OnlineSessionDestroyed_DelegateHandle =
		SessionInterface->AddOnDestroySessionCompleteDelegate_Handle(DestroySessionCompleteDelegate);

	if (!SessionInterface->DestroySession(NAME_GameSession))
	{
		SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnlineSessionDestroyed_DelegateHandle);
		UE_LOG(OnlineSessionSubsystem, Warning, TEXT("Could not destroy a Session!"));
		OnOnlineSessionDestroyed_Event.Broadcast(false);
	}
}

void UOnlineSessionSubsystem::FindSessions(int32 MaxSearchResults)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) { OnOnlineSessionsFound_Event.Broadcast(TArray<FOnlineSessionSearchResult>(), false); return; }

	if (!SessionSearchSettings.IsValid()) InitOnlineSessionSettings();
	SessionSearchSettings->MaxSearchResults = MaxSearchResults;
	
	OnlineSessionsFound_DelegateHandle =
		SessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	if (!SessionInterface->FindSessions(0, SessionSearchSettings.ToSharedRef()))
	{
		SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnlineSessionsFound_DelegateHandle);
		UE_LOG(OnlineSessionSubsystem, Warning, TEXT("Could not find sessions!"));
		OnOnlineSessionsFound_Event.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
	}
}

void UOnlineSessionSubsystem::JoinGameSession(const FOnlineSessionSearchResult& SessionResult)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) { OnOnlineSessionsJoined_Event.Broadcast(EOnJoinSessionCompleteResult::Type::UnknownError); return; }

	OnlineSessionsJoined_DelegateHandle =
		SessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
	
	if (!SessionInterface->JoinSession(0, NAME_GameSession, SessionResult))
	{
		SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnlineSessionsJoined_DelegateHandle);
		UE_LOG(OnlineSessionSubsystem, Warning, TEXT("Could not join a Session!"));
		OnOnlineSessionsJoined_Event.Broadcast(EOnJoinSessionCompleteResult::Type::UnknownError);
	}
}

void UOnlineSessionSubsystem::JoinGameSession(int32 SessionResultIndex)
{
	if (!SessionSearchSettings.IsValid()) { OnOnlineSessionsJoined_Event.Broadcast(EOnJoinSessionCompleteResult::Type::UnknownError); return; }
	if (SessionSearchSettings->SearchResults.Num() <= SessionResultIndex) { OnOnlineSessionsJoined_Event.Broadcast(EOnJoinSessionCompleteResult::Type::UnknownError); return; }
	JoinGameSession(SessionSearchSettings->SearchResults[SessionResultIndex]);
}

bool UOnlineSessionSubsystem::HaveOpenedSession()
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	return SessionInterface->GetNamedSession(NAME_GameSession) != nullptr;
}

#pragma endregion

#pragma region Protected_Functions

void UOnlineSessionSubsystem::InitOnlineSessionSettings()
{
	// Parameters from Project Settings should be used here

	UVRBaseMultiplayerSettings* GameSettings = GetMutableDefault<UVRBaseMultiplayerSettings>();
	if (!GameSettings) { ensure(GameSettings); return; }

	bool bIsLan = false;
#if WITH_EDITOR
	bIsLan = GameSettings ? GameSettings->bUseLanForEditorBuilds : false;
#endif

	// Creating session structs 

	if (!SessionSettings.IsValid())
	{
		SessionSettings = MakeShareable(new FOnlineSessionSettings());

		SessionSettings->bIsDedicated = false;
		SessionSettings->bAllowInvites = false;
		SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

		SessionSettings->NumPrivateConnections = 0;
		SessionSettings->bAllowJoinInProgress = true;
		SessionSettings->bAllowJoinViaPresence = true;
		SessionSettings->bUsesPresence = true;
		SessionSettings->bIsLANMatch = bIsLan;
		SessionSettings->bShouldAdvertise = true;
		SessionSettings->bUseLobbiesIfAvailable = true; // Should be used since UE4.27
		
		if(GameSettings->SubsystemType == EOnlineSubsystemType::EOS) SessionSettings->bUseLobbiesVoiceChatIfAvailable = GameSettings->bUseEOSVoiceChat;

		if (!GameSettings->CustomSessionQueryKey.IsNone())
			SessionSettings->Set(GameSettings->CustomSessionQueryKey, GameSettings->CustomSessionQueryKey.ToString(),
				EOnlineDataAdvertisementType::ViaOnlineService);
	}

	if (!SessionSearchSettings.IsValid())
	{
		SessionSearchSettings = MakeShareable(new FOnlineSessionSearch());
		
		SessionSearchSettings->bIsLanQuery = bIsLan;
		SessionSearchSettings->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

		if (GameSettings->SubsystemType == EOnlineSubsystemType::EOS)
			SessionSearchSettings->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

		if (!GameSettings->CustomSessionQueryKey.IsNone())
			SessionSearchSettings->QuerySettings.Set(GameSettings->CustomSessionQueryKey, GameSettings->CustomSessionQueryKey.ToString(),
				EOnlineComparisonOp::Equals);
	}

	if (GameSettings->bAutoLogin)
	{
		LoginCompleteDelegate.BindLambda([this](int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error) {

			bHasAutoLoginCompleted = bWasSuccessful;
			if (auto IdentityInterface = Online::GetIdentityInterface())
				IdentityInterface->ClearOnLoginCompleteDelegate_Handle(0, LoginComplete_DelegateHandle);
			UE_LOG(OnlineSessionSubsystem, Warning, TEXT("AutoLogin With Success: %d, UserId: %s, ErrorStr: "), bWasSuccessful ? 1 : 0, *UserId.ToString(), *Error);
		});

		if (auto IdentityInterface = Online::GetIdentityInterface())
		{
			LoginComplete_DelegateHandle = IdentityInterface->AddOnLoginCompleteDelegate_Handle(0, LoginCompleteDelegate);
			IdentityInterface->AutoLogin(0);
		}
	}
}

#pragma endregion

#pragma region Private_Functions

void UOnlineSessionSubsystem::OnOnlineSession_Created(FName SessionName, bool Successful)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return;

	if (Successful) { UE_LOG(OnlineSessionSubsystem, Log, TEXT("Session created: %s"), *SessionName.ToString()); }

	SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnlineSessionCreated_DelegateHandle);
	OnOnlineSessionCreated_Event.Broadcast(Successful);
}

void UOnlineSessionSubsystem::OnOnlineSession_Started(FName SessionName, bool Successful)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return;

	if (Successful) { UE_LOG(OnlineSessionSubsystem, Log, TEXT("Session started: %s"), *SessionName.ToString()); }

	SessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnlineSessionStarted_DelegateHandle);
	OnOnlineSessionStarted_Event.Broadcast(Successful);
}

void UOnlineSessionSubsystem::OnOnlineSession_Destroyed(FName SessionName, bool Successful)
{
	SessionDestructionInProgress = false;
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return;

	if (Successful) { UE_LOG(OnlineSessionSubsystem, Log, TEXT("Session %s was destroyed"), *SessionName.ToString()); }

	SessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnlineSessionDestroyed_DelegateHandle);
	OnOnlineSessionDestroyed_Event.Broadcast(Successful);
}

void UOnlineSessionSubsystem::OnFindSessionsCompleted(bool Successful)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return;

	if (Successful) { UE_LOG(OnlineSessionSubsystem, Log, TEXT("Find Session search results found: %d"), SessionSearchSettings->SearchResults.Num()); }
	
	SessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnlineSessionsFound_DelegateHandle);
	OnOnlineSessionsFound_Event.Broadcast(SessionSearchSettings->SearchResults, Successful);
}

void UOnlineSessionSubsystem::OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return;

	UE_LOG(OnlineSessionSubsystem, Log, TEXT("Join Session completed with result %d"), Result);

	SessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnlineSessionsJoined_DelegateHandle);

	OnOnlineSessionsJoined_Event.Broadcast(Result);
}

bool UOnlineSessionSubsystem::TryToClientTravelToCurrentSession(FString& ClientOptions)
{
	auto SessionInterface = Online::GetSessionInterface(GetWorld());
	if (!ensure(SessionInterface)) return false;

	FString ConnectString;
	if (!SessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString)) return false;

	UE_LOG(OnlineSessionSubsystem, Log, TEXT("TryToTravelToCurrentSession() with connect String: %s"), *ConnectString);

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
		PlayerController->ClientTravel(ConnectString + ClientOptions, TRAVEL_Absolute);

	return true;
}

const FUniqueNetId& UOnlineSessionSubsystem::GetLocalPlayerUniqueNetID() const
{
	const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();

	UE_LOG(OnlineSessionSubsystem, Log, TEXT("Current Local Player NetID: %s"), *LocalPlayer->GetPreferredUniqueNetId().ToString());

	return *LocalPlayer->GetPreferredUniqueNetId();
}

#pragma endregion