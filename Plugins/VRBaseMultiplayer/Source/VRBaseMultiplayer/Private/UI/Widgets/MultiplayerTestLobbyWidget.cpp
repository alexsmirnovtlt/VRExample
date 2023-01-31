// 2021-2022 Alexander Smirnov


#include "UI/Widgets/MultiplayerTestLobbyWidget.h"

#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

#include "HeadMountedDisplayFunctionLibrary.h"
#include "OnlineSessionSettings.h"
#include "Misc/CommandLine.h"

#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/CheckBox.h"
#include "Components/Button.h"

#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"
#include "Gameplay/Interfaces/GameInstance/NetworkFailureProvider.h"
#include "UI/Widgets/Items/FoundSessionItemWidget.h"

void UMultiplayerTestLobbyWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Button_Host->OnReleased.AddDynamic(this, &UMultiplayerTestLobbyWidget::OnButtonClicked_Host);
	Button_SearchGames->OnReleased.AddDynamic(this, &UMultiplayerTestLobbyWidget::OnButtonClicked_SearchGames);
	Button_JoinGame->OnReleased.AddDynamic(this, &UMultiplayerTestLobbyWidget::OnButtonClicked_JoinGame);
	Button_PlayOffline->OnReleased.AddDynamic(this, &UMultiplayerTestLobbyWidget::OnButtonClicked_PlayOffline);

	auto GameInstance = GetGameInstance();

	OnlineSessionSubsystem = GameInstance->GetSubsystem<UOnlineSessionSubsystem>();
	if (OnlineSessionSubsystem)
	{
		if (OnlineSessionSubsystem->IsSessionDestructionInProgress())
		{
			ShowLoadingIndicator(true);
			CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionDestroyed_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnCurrentSessionDestroyed);
		}
		else if (OnlineSessionSubsystem->HaveOpenedSession())
		{
			// Closing currently opened session
			ShowLoadingIndicator(true);
			CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionDestroyed_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnCurrentSessionDestroyed);
			OnlineSessionSubsystem->DestroySession();
		}
	}
	else ensure(false);

	if(GameInstance->Implements<UNetworkFailureProvider>())
	{
		if (INetworkFailureProvider::Execute_HaveCachedNetworkError(GameInstance))
		{
			// Show widget with error text if player somehow got disconnected
			OnNetworkConnectionFailure(
				INetworkFailureProvider::Execute_GetAndResetLastClientConnectionError(GameInstance),
				INetworkFailureProvider::Execute_GetAndResetLastClientConnectionErrorString(GameInstance)
			);
		}
	}
}

void UMultiplayerTestLobbyWidget::RemoveFromParent()
{
	Super::RemoveFromParent();

	if (CurrentHandle.IsValid())
	{
		if (OnlineSessionSubsystem->OnOnlineSessionDestroyed_Event.Remove(CurrentHandle)) return;
		if (OnlineSessionSubsystem->OnOnlineSessionsFound_Event.Remove(CurrentHandle)) return;
		if (OnlineSessionSubsystem->OnOnlineSessionCreated_Event.Remove(CurrentHandle)) return;
		if (OnlineSessionSubsystem->OnOnlineSessionsJoined_Event.Remove(CurrentHandle)) return;
		if (OnlineSessionSubsystem->OnOnlineSessionStarted_Event.Remove(CurrentHandle)) return;
	}
}

// BEGIN Button click handling

void UMultiplayerTestLobbyWidget::OnButtonClicked_Host()
{
	Button_Host->SetIsEnabled(false);
	ShowLoadingIndicator(true);

	CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionCreated_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnSessonCreated);
	OnlineSessionSubsystem->CreateSession(NumPublicConnections, EditableText_SessionName->GetText().ToString(), MapToOpen.ToSoftObjectPath().GetLongPackageName());
}

void UMultiplayerTestLobbyWidget::OnButtonClicked_SearchGames()
{
	ShowLoadingIndicator(true);
	ScrollBox_FoundGames->ClearChildren();
	Button_JoinGame->SetIsEnabled(false);
	Button_SearchGames->SetIsEnabled(false);

	CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionsFound_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnGameSessionFound);

	OnlineSessionSubsystem->FindSessions(MaxSessionSearchResults);
}

void UMultiplayerTestLobbyWidget::OnButtonClicked_JoinGame()
{
	ShowLoadingIndicator(true);
	Button_JoinGame->SetIsEnabled(false);

	CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionsJoined_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnGameSessionJoined);
	OnlineSessionSubsystem->JoinGameSession(SelectedItem->GetSessionItemIndex());
}

void UMultiplayerTestLobbyWidget::OnButtonClicked_PlayOffline()
{
	UGameplayStatics::OpenLevel(GetWorld(), FName(MapToOpen.ToSoftObjectPath().GetLongPackageName()), true);
}

void UMultiplayerTestLobbyWidget::OnSessionItemClicked(UFoundSessionItemWidget* Item)
{
	Button_JoinGame->SetIsEnabled(true);

	if (SelectedItem) SelectedItem->SetIsEnabled(true);
	SelectedItem = Item;
	SelectedItem->SetIsEnabled(false);
}

// END Button click handling

// BEGIN Online subsystem related

void UMultiplayerTestLobbyWidget::OnSessonCreated(bool bSuccess)
{
	ShowLoadingIndicator(false);
	OnlineSessionSubsystem->OnOnlineSessionCreated_Event.Remove(CurrentHandle);
	if (!bSuccess) return;
	
	CurrentHandle = OnlineSessionSubsystem->OnOnlineSessionStarted_Event.AddUObject(this, &UMultiplayerTestLobbyWidget::OnSessonStarted);
	OnlineSessionSubsystem->StartSession();
}

void UMultiplayerTestLobbyWidget::OnSessonStarted(bool bSuccess)
{
	ShowLoadingIndicator(false);
	Button_Host->SetIsEnabled(true);
	OnlineSessionSubsystem->OnOnlineSessionStarted_Event.Remove(CurrentHandle);
	CurrentHandle.Reset();

	if (!bSuccess) return;

	FString TravelURL = MapToOpen.ToSoftObjectPath().GetLongPackageName() + "?listen" + AppendAdditionalOptionsAsServer();

	GetWorld()->ServerTravel(TravelURL, true);
}

void UMultiplayerTestLobbyWidget::OnCurrentSessionDestroyed(bool bSuccess)
{
	ShowLoadingIndicator(false);
	OnlineSessionSubsystem->OnOnlineSessionDestroyed_Event.Remove(CurrentHandle);
	CurrentHandle.Reset();
}

void UMultiplayerTestLobbyWidget::OnGameSessionFound(const TArray<FOnlineSessionSearchResult>& SessionResults, bool Successful)
{
	ShowLoadingIndicator(false);
	OnlineSessionSubsystem->OnOnlineSessionsFound_Event.Remove(CurrentHandle);
	CurrentHandle.Reset();
	Button_SearchGames->SetIsEnabled(true);

	if (!Successful) return;

	for (int32 i = 0; i < SessionResults.Num(); ++i)
	{
		auto NewItem = CreateWidget<UFoundSessionItemWidget>(this, FoundSessionItemClass);
		NewItem->Init(i, SessionResults[i], this);
		ScrollBox_FoundGames->AddChild(NewItem);
	}
}

void UMultiplayerTestLobbyWidget::OnGameSessionJoined(EOnJoinSessionCompleteResult::Type Result)
{
	OnlineSessionSubsystem->OnOnlineSessionsJoined_Event.Remove(CurrentHandle);
	Button_JoinGame->SetIsEnabled(true);
	CurrentHandle.Reset();

	if (Result == EOnJoinSessionCompleteResult::Type::Success)
	{
		FString Options = "";
		const FString CMDArgs = FString(FCommandLine::Get());

		for (auto& Value : CommandLineArgsToOptions)
		{
			const FString ValueStr = Value.ToString();
			if(CMDArgs.Contains("-" + ValueStr)) Options += "?" + ValueStr;
		}

		Options += AppendAdditionalOptionsAsClient(); // BPs may add additional data, f.e. FullBody players appends height and asset path here
		
		OnlineSessionSubsystem->TryToClientTravelToCurrentSession(Options);	
	}
	else OnJoinSessionFailure((uint8)Result);
}

// END Online subsystem related