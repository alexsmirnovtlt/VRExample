// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

//#include "OnlineSessionSettings.h"

#include "FoundSessionItemWidget.generated.h"

class FOnlineSessionSearchResult;
class UOnlineSessionSubsystem;

/**
 * Default item that shows found online game sessions, tightly coupled to UMultiplayerTestLobbyWidget
 */
UCLASS(Abstract)
class VRBASEMULTIPLAYER_API UFoundSessionItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void Init(int32 Index, const FOnlineSessionSearchResult& SearchResult, class UMultiplayerTestLobbyWidget* MainWidgetPtr);
	int32 GetSessionItemIndex() { return SessionItemIndex; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* MainText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UButton* Button_Main;

	static FText DefaultSessionName;
	static FText FoundSessionFormatText;

	UFUNCTION()
	void OnButtonPressed();

	virtual void SetupText(const FOnlineSessionSearchResult& SearchResult);

	UPROPERTY()
	class UMultiplayerTestLobbyWidget* MainWidget;
	int32 SessionItemIndex;
};
