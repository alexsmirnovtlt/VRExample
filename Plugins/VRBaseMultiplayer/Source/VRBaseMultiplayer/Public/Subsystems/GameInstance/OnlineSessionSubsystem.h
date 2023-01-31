#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "General/Structs/MultiplayerStructs.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"

#include "OnlineSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnOnlineSessionActionPerformedDelegate, bool);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnOnlineSessionsFoundDelegate, const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnJoinOnlineSessionCompleteDelegate, EOnJoinSessionCompleteResult::Type Result);

DECLARE_LOG_CATEGORY_EXTERN(OnlineSessionSubsystem, Log, All);

/**
 * Subsystem that handles networking
 */
UCLASS(/*BlueprintType, Blueprintable*/)
class VRBASEMULTIPLAYER_API UOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

	// TODO With some effort most logic can be made BP accessible, including assignable events and such
	// TODO Delegate handles could have been handled better (pun), there is really no need to have them in the first place

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const;

	FOnOnlineSessionActionPerformedDelegate OnOnlineSessionCreated_Event;
	FOnOnlineSessionActionPerformedDelegate OnOnlineSessionStarted_Event;
	FOnOnlineSessionActionPerformedDelegate OnOnlineSessionDestroyed_Event;
	FOnOnlineSessionsFoundDelegate OnOnlineSessionsFound_Event;
	FOnJoinOnlineSessionCompleteDelegate OnOnlineSessionsJoined_Event;

	void CreateSession(int32 NumPublicConnections, const FString& CustomSessionName, const FString& MapName, const TArray<FOnlineSessionSettingsKeyPair> AdditionalSettings = TArray<FOnlineSessionSettingsKeyPair>());
	void StartSession();
	void DestroySession();
	void FindSessions(int32 MaxSearchResults);
	void JoinGameSession(const FOnlineSessionSearchResult& SessionResult);
	void JoinGameSession(int32 SessionResultIndex);

	bool HaveOpenedSession();
	bool TryToClientTravelToCurrentSession(FString& ClientOptions);

	static const FName CustomSessionQueryKey_SessionName;

	bool IsSessionDestructionInProgress() { return SessionDestructionInProgress; };

protected:

	const FUniqueNetId& GetLocalPlayerUniqueNetID() const;

	virtual void InitOnlineSessionSettings();

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate StartSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate DestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FOnLoginCompleteDelegate LoginCompleteDelegate;

	void OnOnlineSession_Created(FName SessionName, bool Successful);
	void OnOnlineSession_Started(FName SessionName, bool Successful);
	void OnOnlineSession_Destroyed(FName SessionName, bool Successful);
	void OnFindSessionsCompleted(bool Successful);
	void OnJoinSessionCompleted(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	FDelegateHandle OnlineSessionCreated_DelegateHandle;
	FDelegateHandle OnlineSessionStarted_DelegateHandle;
	FDelegateHandle OnlineSessionDestroyed_DelegateHandle;
	FDelegateHandle OnlineSessionsFound_DelegateHandle;
	FDelegateHandle OnlineSessionsJoined_DelegateHandle;
	FDelegateHandle LoginComplete_DelegateHandle;

	TSharedPtr<FOnlineSessionSettings> SessionSettings;
	TSharedPtr<FOnlineSessionSearch> SessionSearchSettings;

	bool SessionDestructionInProgress = false;
	bool bHasAutoLoginCompleted = false;
};
