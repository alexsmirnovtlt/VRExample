// 2021-2022 Alexander Smirnov


#include "UI/Widgets/Items/FoundSessionItemWidget.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "UI/Widgets/MultiplayerTestLobbyWidget.h"
#include "Subsystems/GameInstance/OnlineSessionSubsystem.h"

#define LOCTEXT_NAMESPACE "OnlineSubsystem"

FText UFoundSessionItemWidget::DefaultSessionName = LOCTEXT("OnlineSubsystem.undefined_session_name", "Unknown");
FText UFoundSessionItemWidget::FoundSessionFormatText = LOCTEXT("OnlineSubsystem.found_session_format", "({0}/{1}) {2} | Host: {3}");

void UFoundSessionItemWidget::Init(int32 Index, const FOnlineSessionSearchResult& SearchResult, UMultiplayerTestLobbyWidget* MainWidgetPtr)
{
	Button_Main->OnPressed.AddDynamic(this, &UFoundSessionItemWidget::OnButtonPressed);
	MainWidget = MainWidgetPtr;
	SessionItemIndex = Index;
	SetupText(SearchResult);
}

void UFoundSessionItemWidget::OnButtonPressed()
{
	if (MainWidget) MainWidget->OnSessionItemClicked(this);
}

void UFoundSessionItemWidget::SetupText(const FOnlineSessionSearchResult& SearchResult)
{
	FString GameName = "";
	const FName& SessionName = UOnlineSessionSubsystem::CustomSessionQueryKey_SessionName;
	const auto& SessionSettings = SearchResult.Session.SessionSettings;

	if (SessionSettings.Settings.Contains(SessionName))
		SessionSettings.Get(SessionName, GameName);
	if(GameName.IsEmpty()) GameName = DefaultSessionName.ToString();

	// Adding info about current\max players and host name
	const int32 MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
	const int32 CurrentPlayers = MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
	const FString HostPlayerName = SearchResult.Session.OwningUserName;

	MainText->SetText(
		FText::Format(FoundSessionFormatText, CurrentPlayers, MaxPlayers, FText::FromString(GameName), FText::FromString(HostPlayerName))
	);
}

#undef LOCTEXT_NAMESPACE