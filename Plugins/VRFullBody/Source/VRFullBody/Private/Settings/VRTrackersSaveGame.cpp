// 2021-2022 Alexander Smirnov


#include "Settings/VRTrackersSaveGame.h"

#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

const FString UVRTrackersSaveGame::SlotName = TEXT("VRTrackersDataSave");

void UVRTrackersSaveGame::ApplyOffsetsFromSave(AVRFullBodyPawn_MoreThan3DoF* Pawn)
{
	ensure(Pawn);
	if (!IsValid(Pawn)) return;

	Pawn->Tracker_Waist_Offset->SetRelativeTransform(FTransform(Tracker_Waist_Offset.GetRotation(), Tracker_Waist_Offset.GetLocation()));
	Pawn->Tracker_Chest_Offset->SetRelativeTransform(FTransform(Tracker_Chest_Offset.GetRotation(), Tracker_Chest_Offset.GetLocation()));
	Pawn->Tracker_Elbow_Right_Offset->SetRelativeTransform(FTransform(Tracker_Elbow_Right_Offset.GetRotation(), Tracker_Elbow_Right_Offset.GetLocation()));
	Pawn->Tracker_Elbow_Left_Offset->SetRelativeTransform(FTransform(Tracker_Elbow_Left_Offset.GetRotation(), Tracker_Elbow_Left_Offset.GetLocation()));
	Pawn->Tracker_Knee_Right_Offset->SetRelativeTransform(FTransform(Tracker_Knee_Right_Offset.GetRotation(), Tracker_Knee_Right_Offset.GetLocation()));
	Pawn->Tracker_Knee_Left_Offset->SetRelativeTransform(FTransform(Tracker_Knee_Left_Offset.GetRotation(), Tracker_Knee_Left_Offset.GetLocation()));
	Pawn->Tracker_Foot_Right_Offset->SetRelativeTransform(FTransform(Tracker_Foot_Right_Offset.GetRotation(), Tracker_Foot_Right_Offset.GetLocation()));
	Pawn->Tracker_Foot_Left_Offset->SetRelativeTransform(FTransform(Tracker_Foot_Left_Offset.GetRotation(), Tracker_Foot_Left_Offset.GetLocation()));
}