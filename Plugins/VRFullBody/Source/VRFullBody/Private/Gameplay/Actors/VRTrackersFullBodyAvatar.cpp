// 2021-2022 Alexander Smirnov


#include "Gameplay/Actors/VRTrackersFullBodyAvatar.h"

#include "Gameplay/CoreManagers/Subsystems/GameInstance/PersistentVRDataSubsystem.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"
#include "Settings/VRTrackersSaveGame.h"

void AVRTrackersFullBodyAvatar::Tick(float DeltaTime)
{
	if (bUpdateAvatarTransform) Super::Tick(DeltaTime);
	else Super::Super::Tick(DeltaTime); // Skip avatar movement and rotation when avatar is in calibration state
}

void AVRTrackersFullBodyAvatar::SetupAffectorsForLocalPlayer_Internal()
{
	Super::SetupAffectorsForLocalPlayer_Internal();

	if (auto SaveGameSubsystem = GetGameInstance()->GetSubsystem<UPersistentVRDataSubsystem>())
	{
		if (auto TrackersData = SaveGameSubsystem->GetVRTrackersSaveGame())
		{
			if (auto TrackersPawn = Cast<AVRFullBodyPawn_MoreThan3DoF>(FullbodyPawn))
			{
				TrackersData->ApplyOffsetsFromSave(TrackersPawn);
				TrackersPawn->ReinitTrackedBonesMap(TrackersData->TrackedBonesMap); // Not an effector but if we are loading saved game might as well apply it here
			}
		}
	}
}

void AVRTrackersFullBodyAvatar::UpdateActorLocation(float DeltaTime)
{
	SetActorLocation(GetDesiredWorldLocation());
}

void AVRTrackersFullBodyAvatar::UpdateActorRotation(float DeltaTime)
{	
	SetActorRotation(FullbodyPawn->GetPlayerForwardWorldRotation());
}