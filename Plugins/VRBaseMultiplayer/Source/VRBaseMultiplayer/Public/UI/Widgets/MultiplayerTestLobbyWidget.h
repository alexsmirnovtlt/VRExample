// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Interfaces/OnlineSessionInterface.h"
#include "Engine/EngineBaseTypes.h"

#include "MultiplayerTestLobbyWidget.generated.h"

class UButton;
class UCheckBox;
class UScrollBox;
class UEditableText;

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API UMultiplayerTestLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION() void OnSessionItemClicked(UFoundSessionItemWidget* Item);

protected:

	virtual void NativeOnInitialized() override;
	virtual void RemoveFromParent() override;

	UFUNCTION(BlueprintCallable)
	void SetLevelToOpen(const TSoftObjectPtr<UWorld>& NewLevelSoftPtr) { MapToOpen = NewLevelSoftPtr; };
	// If any of those are found in the Command-line arguments as a connecting client, add them to client`s Options string 
	UFUNCTION(BlueprintCallable)
	void SetCommandLineArgs(const TArray<FName> NewCommandLineArgsToOptions) { CommandLineArgsToOptions = NewCommandLineArgsToOptions; };

	UPROPERTY(EditDefaultsOnly, Category = "Default settings")
	TSubclassOf<class UFoundSessionItemWidget> FoundSessionItemClass;
	TArray<FName> CommandLineArgsToOptions;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_Host;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_JoinGame;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_SearchGames;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button_PlayOffline;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScrollBox* ScrollBox_FoundGames;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableText* EditableText_SessionName;

	UFUNCTION() void OnButtonClicked_Host();
	UFUNCTION() void OnButtonClicked_SearchGames();
	UFUNCTION() void OnButtonClicked_JoinGame();
	UFUNCTION() void OnButtonClicked_PlayOffline();

	UFoundSessionItemWidget* SelectedItem;

	// BEGIN Online subsystem related

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Default settings - OnlineSubsystem")
	int32 NumPublicConnections = 5;
	UPROPERTY(EditDefaultsOnly, Category = "Default settings - OnlineSubsystem")
	int32 MaxSessionSearchResults = 10;

	UFUNCTION(BlueprintNativeEvent)
	void ShowLoadingIndicator(bool bVisible);
	virtual void ShowLoadingIndicator_Implementation(bool bVisible) {};

	TSoftObjectPtr<class UWorld> MapToOpen;

	UFUNCTION(BlueprintImplementableEvent)
	void OnNetworkConnectionFailure(const TEnumAsByte<ENetworkFailure::Type>& ErrorType, const FString& ErrorMsg);
	// Unfortunately EOnJoinSessionCompleteResult is not exposed for BPs
	UFUNCTION(BlueprintImplementableEvent)
	void OnJoinSessionFailure(uint8 OnJoinSessionCompleteResult);

	void OnSessonCreated(bool bSuccess);
	void OnSessonStarted(bool bSuccess);
	void OnCurrentSessionDestroyed(bool bSuccess);
	void OnGameSessionFound(const TArray<class FOnlineSessionSearchResult>& SessionResults, bool Successful);
	void OnGameSessionJoined(EOnJoinSessionCompleteResult::Type Result);

	UFUNCTION(BlueprintNativeEvent, Category = "Overridable")
	FString AppendAdditionalOptionsAsClient();
	virtual FString AppendAdditionalOptionsAsClient_Implementation() { return FString(); };
	UFUNCTION(BlueprintNativeEvent, Category = "Overridable")
	FString AppendAdditionalOptionsAsServer();
	virtual FString AppendAdditionalOptionsAsServer_Implementation() { return FString(); };

private:
	class UOnlineSessionSubsystem* OnlineSessionSubsystem;

	FDelegateHandle CurrentHandle;

	// END Online subsystem related
};
