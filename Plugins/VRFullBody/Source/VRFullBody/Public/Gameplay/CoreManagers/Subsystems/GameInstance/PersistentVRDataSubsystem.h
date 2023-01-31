// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PersistentVRDataSubsystem.generated.h"

class UVRFullBodyDataSaveGame;
class UVRTrackersSaveGame;
/**
 * 
 */
UCLASS()
class VRFULLBODY_API UPersistentVRDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	UVRFullBodyDataSaveGame* GetSaveGameData();

	// Lazily load save game for trackers offsets
	UFUNCTION(BlueprintCallable)
	UVRTrackersSaveGame* GetVRTrackersSaveGame();

	UFUNCTION(BlueprintCallable)
	void ResetAllSavedData();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Persistent VR Data Subsystem")
	UVRFullBodyDataSaveGame* SaveGameData;
	UPROPERTY()
	UVRTrackersSaveGame* TrackersSaveGameData; // Not exposed to BPs because it gets loaded on demand at GetVRTrackersSaveGame()
};
