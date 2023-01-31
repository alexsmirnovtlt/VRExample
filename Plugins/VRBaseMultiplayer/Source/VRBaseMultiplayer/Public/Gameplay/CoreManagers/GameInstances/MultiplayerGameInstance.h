// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "Gameplay/Interfaces/GameInstance/NetworkFailureProvider.h"
#include "Gameplay/Interfaces/GameInstance/LoadingScreenProvider.h"

#include "MultiplayerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRBASEMULTIPLAYER_API UMultiplayerGameInstance : public UGameInstance, public INetworkFailureProvider, public ILoadingScreenProvider
{
	GENERATED_BODY()

protected:
	virtual void Init() override;
	virtual void OnWorldChanged(UWorld* OldWorld, UWorld* NewWorld);

protected:
	UFUNCTION()
	void OnNetworkFailureHappened(UWorld* World, UNetDriver* Driver, ENetworkFailure::Type FailureType, const FString& ErrorStr);

	bool bShowBlackScreenOnLevelChange = false;

	// INetworkFailureProvider interface 
	virtual bool HaveCachedNetworkError_Implementation() override { return LastNetworkError != 0xFF; }
	virtual ENetworkFailure::Type GetAndResetLastClientConnectionError_Implementation() override;
	virtual FString GetAndResetLastClientConnectionErrorString_Implementation() override;
	uint8 LastNetworkError;
	FString LastNetworkErrorStr;
	// ~ INetworkFailureProvider interface 

	// ILoadingScreenProvider interface 
	virtual void LevelLoadingScreen_Show_Implementation() override;
	virtual void LevelLoadingScreen_Hide_Implementation() override;
	virtual void ShowLevelLoadingScreenOnLevelChange_Implementation(bool bShow) { bShowBlackScreenOnLevelChange = bShow; }
	
	TSharedPtr<class SVRLoadingScreenWidget> VRLoadingScreenWidget;
	TSharedPtr<class SWeakWidget> VRLoadingScreenWidgetContainer;
	// ~ ILoadingScreenProvider interface 
};