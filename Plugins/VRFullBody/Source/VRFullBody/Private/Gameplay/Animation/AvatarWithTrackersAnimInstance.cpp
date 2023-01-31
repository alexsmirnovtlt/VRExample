// 2021-2022 Alexander Smirnov


#include "Gameplay/Animation/AvatarWithTrackersAnimInstance.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

bool UAvatarWithTrackersAnimInstance::GetPawnAndAvatarReferences()
{
	if (IsValid(TrackersPawn)) return true;
	if (Super::GetPawnAndAvatarReferences())
	{
		TrackersPawn = Cast<AVRFullBodyPawn_MoreThan3DoF>(VRFullBodyPawn);
		HasValidVRData = TrackersPawn != nullptr;
		return HasValidVRData;
	}
	return false;
}

void UAvatarWithTrackersAnimInstance::UpdateClassSpecificAnimData()
{
	if (!GetPawnAndAvatarReferences()) return;

	if (TrackersPawn->IsLocallyControlled())
	{
		BoneTransform_Waist = TrackersPawn->Tracker_Waist_Offset->GetComponentTransform();
		BoneTransform_Chest = TrackersPawn->Tracker_Chest_Offset->GetComponentTransform();
		BoneTransform_Elbow_Right = TrackersPawn->Tracker_Elbow_Right_Offset->GetComponentTransform();
		BoneTransform_Elbow_Left = TrackersPawn->Tracker_Elbow_Left_Offset->GetComponentTransform();
		BoneTransform_Knee_Right = TrackersPawn->Tracker_Knee_Right_Offset->GetComponentTransform();
		BoneTransform_Knee_Left = TrackersPawn->Tracker_Knee_Left_Offset->GetComponentTransform();
		BoneTransform_Foot_Right = TrackersPawn->Tracker_Foot_Right_Offset->GetComponentTransform();
		BoneTransform_Foot_Left = TrackersPawn->Tracker_Foot_Left_Offset->GetComponentTransform();
	}
	else
	{
		BoneTransform_Waist = TrackersPawn->Tracker_Waist->GetComponentTransform();
		BoneTransform_Chest = TrackersPawn->Tracker_Chest->GetComponentTransform();
		BoneTransform_Elbow_Right = TrackersPawn->Tracker_Elbow_Right->GetComponentTransform();
		BoneTransform_Elbow_Left = TrackersPawn->Tracker_Elbow_Left->GetComponentTransform();
		BoneTransform_Knee_Right = TrackersPawn->Tracker_Knee_Right->GetComponentTransform();
		BoneTransform_Knee_Left = TrackersPawn->Tracker_Knee_Left->GetComponentTransform();
		BoneTransform_Foot_Right = TrackersPawn->Tracker_Foot_Right->GetComponentTransform();
		BoneTransform_Foot_Left = TrackersPawn->Tracker_Foot_Left->GetComponentTransform();
	}

	bTrackersAvailable_Elbows = TrackersPawn->bTrackersAvailable_Elbows;
	bTrackersAvailable_Knees = TrackersPawn->bTrackersAvailable_Knees;
	bTrackersAvailable_Feet = TrackersPawn->bTrackersAvailable_Feet;
	bTrackerAvailable_Waist = TrackersPawn->bTrackerAvailable_Waist;
	bTrackerAvailable_Chest = TrackersPawn->bTrackerAvailable_Chest;
}