// 2021-2022 Alexander Smirnov


#include "Gameplay/CoreManagers/Subsystems/GameInstance/PersistentVRDataSubsystem.h"

#include "Kismet/GameplayStatics.h"

#include "Settings/VRFullBodyDataSaveGame.h"
#include "Settings/VRTrackersSaveGame.h"

void UPersistentVRDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	GetSaveGameData();
}

UVRTrackersSaveGame* UPersistentVRDataSubsystem::GetVRTrackersSaveGame()
{
	if (IsValid(TrackersSaveGameData)) return TrackersSaveGameData;

	if(UGameplayStatics::DoesSaveGameExist(UVRTrackersSaveGame::SlotName, 0))
	{
		if (auto SavedGameBase = UGameplayStatics::LoadGameFromSlot(UVRTrackersSaveGame::SlotName, 0))
		{
			TrackersSaveGameData = Cast<UVRTrackersSaveGame>(SavedGameBase);
			if(TrackersSaveGameData) return TrackersSaveGameData;
		}
	}

	TrackersSaveGameData = NewObject<UVRTrackersSaveGame>();
	return TrackersSaveGameData;
}

UVRFullBodyDataSaveGame* UPersistentVRDataSubsystem::GetSaveGameData()
{
	if (IsValid(SaveGameData)) return SaveGameData;

	if (UGameplayStatics::DoesSaveGameExist(UVRFullBodyDataSaveGame::SlotName, 0))
	{
		if (auto SavedGameBase = UGameplayStatics::LoadGameFromSlot(UVRFullBodyDataSaveGame::SlotName, 0))
		{
			SaveGameData = Cast<UVRFullBodyDataSaveGame>(SavedGameBase);
			if (SaveGameData)
			{
				if (SaveGameData->AvatarData.AreValuesValid()) return SaveGameData;
				else ensure(false);
			}
		}
	}

	SaveGameData = NewObject<UVRFullBodyDataSaveGame>();
	return SaveGameData;
}

void UPersistentVRDataSubsystem::ResetAllSavedData()
{
	UGameplayStatics::DeleteGameInSlot(UVRFullBodyDataSaveGame::GetAvatarDataSaveSlotName(), 0);
	UGameplayStatics::DeleteGameInSlot(UVRTrackersSaveGame::GetVRTrackersDataSaveSlotName(), 0);

	SaveGameData = nullptr;
	TrackersSaveGameData = nullptr;

	GetSaveGameData();
}