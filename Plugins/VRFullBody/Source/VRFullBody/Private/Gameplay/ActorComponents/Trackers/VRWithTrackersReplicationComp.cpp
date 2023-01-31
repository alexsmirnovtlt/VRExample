// 2021-2022 Alexander Smirnov


#include "Gameplay/ActorComponents/Trackers/VRWithTrackersReplicationComp.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

UVRWithTrackersReplicationComp::UVRWithTrackersReplicationComp()
{
	MaxTrackersServerRPCsPerSecond = 10.f;
	LastTransformsUpdateTime_Trackers = 0.f;
	bShouldLerp_Trackers = false;
}

void UVRWithTrackersReplicationComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Skipping all ticks up to ActorComponent!

	if (!GetVRTrackersPawn()) return;

	if (bShouldLerp_Trackers)
	{
		LastTransformsUpdateTime_Trackers += DeltaTime;
		float LerpModifier = LastTransformsUpdateTime_Trackers / (1.f / MaxServerRPCsPerSecond);
		if (LastTransformsUpdateTime_Trackers >= 1.f)
		{
			LerpModifier = 1.f;
			bShouldLerp_Trackers = false;
		}

		if (!Transform_Waist.GetLocation().IsNearlyZero())
			LerpRelativeTransform(VRTrackersPawn->Tracker_Waist, LastTransform_Waist, Transform_Waist, LerpModifier);
		if (!Transform_Camera.GetLocation().IsNearlyZero())
			LerpRelativeTransform(VRTrackersPawn->MainCamera, LastTransform_Camera, Transform_Camera, LerpModifier);
		
		if (VRTrackersPawn->bTrackerAvailable_Chest)
			LerpRelativeTransform(VRTrackersPawn->Tracker_Chest, LastTransform_Chest, Transform_Chest, LerpModifier);
		if (VRTrackersPawn->bTrackersAvailable_Elbows)
		{
			LerpRelativeTransform(VRTrackersPawn->Tracker_Elbow_Right, LastTransform_Elbow_Right, Transform_Elbow_Right, LerpModifier);
			LerpRelativeTransform(VRTrackersPawn->Tracker_Elbow_Left, LastTransform_Elbow_Left, Transform_Elbow_Left, LerpModifier);
		}		
		if (VRTrackersPawn->bTrackersAvailable_Knees)
		{
			LerpRelativeTransform(VRTrackersPawn->Tracker_Knee_Right, LastTransform_Knee_Right, Transform_Knee_Right, LerpModifier);
			LerpRelativeTransform(VRTrackersPawn->Tracker_Knee_Left, LastTransform_Knee_Left, Transform_Knee_Left, LerpModifier);
		}
		if (VRTrackersPawn->bTrackersAvailable_Feet)
		{
			LerpRelativeTransform(VRTrackersPawn->Tracker_Foot_Right, LastTransform_Foot_Right, Transform_Foot_Right, LerpModifier);
			LerpRelativeTransform(VRTrackersPawn->Tracker_Foot_Left, LastTransform_Foot_Left, Transform_Foot_Left, LerpModifier);
		}
	}

	// Copied from parent but without camera lerp

	if (!bShouldLerp) return;

	LastTransformsUpdateTime += DeltaTime;
	float LerpModifier = LastTransformsUpdateTime / (1.f / MaxServerRPCsPerSecond);
	if (LastTransformsUpdateTime >= 1.f)
	{
		LerpModifier = 1.f;
		bShouldLerp = false;
	}

	if (!Transform_Hand_Right.GetLocation().IsNearlyZero())
		LerpRelativeTransform(VRTrackersPawn->GetGripComponent(true), LastTransform_Hand_Right, Transform_Hand_Right, LerpModifier);
	if (!Transform_Hand_Left.GetLocation().IsNearlyZero())
		LerpRelativeTransform(VRTrackersPawn->GetGripComponent(false), LastTransform_Hand_Left, Transform_Hand_Left, LerpModifier);
}

AVRFullBodyPawn_MoreThan3DoF* UVRWithTrackersReplicationComp::GetVRTrackersPawn()
{
	if (IsValid(VRTrackersPawn)) return VRTrackersPawn;
	VRTrackersPawn = Cast<AVRFullBodyPawn_MoreThan3DoF>(GetVRFullBodyPawn());
	return VRTrackersPawn;
}

bool UVRWithTrackersReplicationComp::GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef)
{
	if (!GetVRTrackersPawn()) { ensure(false); return false; }

	//if (OwnerVRFullBodyPawn->IsCameraDetached()) return false; // TODO Probably not a good logic decision, parent class does the same 

	LastTransformsUpdateTime += DeltaTime; // Limit client to only a number of server RPCs per second
	if (LastTransformsUpdateTime < 1.f / MaxServerRPCsPerSecond) return false;
	LastTransformsUpdateTime -= 1.f / MaxServerRPCsPerSecond;

	// ReportGripLocationForHands bool gets ignored unlike the parent, because it should always be true for fullbody pawns (no detached physical hands)

	FVector Location;
	FRotator Rotation;

	CombineRelative(VRTrackersPawn->GetGripComponent(false), VRTrackersPawn->LocalPlayerBone_Hand_Left, Location, Rotation);
	DataRef.LeftHand.Location = Location;
	DataRef.LeftHand.Rotation = Rotation;

	CombineRelative(VRTrackersPawn->GetGripComponent(true), VRTrackersPawn->LocalPlayerBone_Hand_Right, Location, Rotation);
	DataRef.RightHand.Location = Location;
	DataRef.RightHand.Rotation = Rotation;

	// TODO Two Camera vars are unused! Opportunity to fill it with something like bools from FVRTrackersPlayerData!
	//DataRef.Helm_FloorHeight
	//DataRef.Helm_Rotation 

	if (ConvertDataToWorldSpace) DataRef.RotateByYaw(VRTrackersPawn->GetActorRotation().Yaw);

	return true;
}

bool UVRWithTrackersReplicationComp::GetTrackersDataToReplicate(float DeltaTime, FVRTrackersPlayerData& DataRef)
{
	if (!GetVRTrackersPawn()) { ensure(false); return false; }

	LastTransformsUpdateTime_Trackers += DeltaTime; // Limit client to only a number of server RPCs per second
	if (LastTransformsUpdateTime_Trackers < 1.f / MaxTrackersServerRPCsPerSecond) return false;
	LastTransformsUpdateTime_Trackers -= 1.f / MaxTrackersServerRPCsPerSecond;

	FVector Location;
	FRotator Rotation;

	DataRef.bTrackerAvailable_Waist = VRTrackersPawn->bTrackerAvailable_Waist;
	if (!DataRef.bTrackerAvailable_Waist) return false;

	CombineRelative(VRTrackersPawn->Tracker_Waist, VRTrackersPawn->Tracker_Waist_Offset, Location, Rotation);
	DataRef.Waist.Location = Location;
	DataRef.Waist.Rotation = Rotation;

	DataRef.Camera.Location = VRTrackersPawn->GetCamera()->GetRelativeLocation();
	DataRef.Camera.Rotation = VRTrackersPawn->GetCamera()->GetRelativeRotation();
	
	DataRef.bTrackerAvailable_Chest = VRTrackersPawn->bTrackerAvailable_Chest;
	if (DataRef.bTrackerAvailable_Chest)
	{
		CombineRelative(VRTrackersPawn->Tracker_Chest, VRTrackersPawn->Tracker_Chest_Offset, Location, Rotation);
		DataRef.Chest.Location = Location;
		DataRef.Chest.Rotation = Rotation;
	}

	DataRef.bTrackersAvailable_Elbows = VRTrackersPawn->bTrackersAvailable_Elbows;
	if (DataRef.bTrackersAvailable_Elbows)
	{
		CombineRelative(VRTrackersPawn->Tracker_Elbow_Right, VRTrackersPawn->Tracker_Elbow_Right_Offset, Location, Rotation);
		DataRef.Elbow_Right.Location = Location;
		DataRef.Elbow_Right.Rotation = Rotation;
		CombineRelative(VRTrackersPawn->Tracker_Elbow_Left, VRTrackersPawn->Tracker_Elbow_Left_Offset, Location, Rotation);
		DataRef.Elbow_Left.Location = Location;
		DataRef.Elbow_Left.Rotation = Rotation;
	}

	DataRef.bTrackersAvailable_Knees = VRTrackersPawn->bTrackersAvailable_Knees;
	if (DataRef.bTrackersAvailable_Knees)
	{
		CombineRelative(VRTrackersPawn->Tracker_Knee_Right, VRTrackersPawn->Tracker_Knee_Right_Offset, Location, Rotation);
		DataRef.Knee_Right.Location = Location;
		DataRef.Knee_Right.Rotation = Rotation;
		CombineRelative(VRTrackersPawn->Tracker_Knee_Left, VRTrackersPawn->Tracker_Knee_Left_Offset, Location, Rotation);
		DataRef.Knee_Left.Location = Location;
		DataRef.Knee_Left.Rotation = Rotation;
	}

	DataRef.bTrackersAvailable_Feet = VRTrackersPawn->bTrackersAvailable_Feet;
	if (DataRef.bTrackersAvailable_Feet)
	{
		CombineRelative(VRTrackersPawn->Tracker_Foot_Right, VRTrackersPawn->Tracker_Foot_Right_Offset, Location, Rotation);
		DataRef.Foot_Right.Location = Location;
		DataRef.Foot_Right.Rotation = Rotation;
		CombineRelative(VRTrackersPawn->Tracker_Foot_Left, VRTrackersPawn->Tracker_Foot_Left_Offset, Location, Rotation);
		DataRef.Foot_Left.Location = Location;
		DataRef.Foot_Left.Rotation = Rotation;
	}

	if (ConvertDataToWorldSpace) DataRef.RotateByYaw(VRTrackersPawn->GetActorRotation().Yaw);
	
	return true;
}

void UVRWithTrackersReplicationComp::SetNewTrackersData(FVRTrackersPlayerData& DataRef)
{
	if (!VRTrackersPawn) return;

	if (ConvertDataToWorldSpace) DataRef.RotateByYaw(-VRTrackersPawn->GetActorRotation().Yaw);

	LastTransformsUpdateTime_Trackers = 0.f;
	bShouldLerp_Trackers = true;

	// Set lerp from
	LastTransform_Waist = VRTrackersPawn->Tracker_Waist->GetRelativeTransform();
	LastTransform_Camera = VRTrackersPawn->GetCamera()->GetRelativeTransform();
	LastTransform_Chest = VRTrackersPawn->Tracker_Chest->GetRelativeTransform();
	LastTransform_Elbow_Right = VRTrackersPawn->Tracker_Elbow_Right->GetRelativeTransform();
	LastTransform_Elbow_Left = VRTrackersPawn->Tracker_Elbow_Left->GetRelativeTransform();
	LastTransform_Knee_Right = VRTrackersPawn->Tracker_Knee_Right->GetRelativeTransform();
	LastTransform_Knee_Left = VRTrackersPawn->Tracker_Knee_Left->GetRelativeTransform();
	LastTransform_Foot_Right = VRTrackersPawn->Tracker_Foot_Right->GetRelativeTransform();
	LastTransform_Foot_Left = VRTrackersPawn->Tracker_Foot_Left->GetRelativeTransform();
	// Set lerp to
	Transform_Waist = FTransform(DataRef.Waist.Rotation, DataRef.Waist.Location);
	Transform_Camera = FTransform(DataRef.Camera.Rotation, DataRef.Camera.Location);
	Transform_Chest = FTransform(DataRef.Chest.Rotation, DataRef.Chest.Location);
	Transform_Elbow_Right = FTransform(DataRef.Elbow_Right.Rotation, DataRef.Elbow_Right.Location);
	Transform_Elbow_Left = FTransform(DataRef.Elbow_Left.Rotation, DataRef.Elbow_Left.Location);
	Transform_Knee_Right = FTransform(DataRef.Knee_Right.Rotation, DataRef.Knee_Right.Location);
	Transform_Knee_Left = FTransform(DataRef.Knee_Left.Rotation, DataRef.Knee_Left.Location);
	Transform_Foot_Right = FTransform(DataRef.Foot_Right.Rotation, DataRef.Foot_Right.Location);
	Transform_Foot_Left = FTransform(DataRef.Foot_Left.Rotation, DataRef.Foot_Left.Location);
	
	VRTrackersPawn->bTrackersAvailable_Elbows = DataRef.bTrackersAvailable_Elbows;
	VRTrackersPawn->bTrackersAvailable_Knees = DataRef.bTrackersAvailable_Knees;
	VRTrackersPawn->bTrackersAvailable_Feet = DataRef.bTrackersAvailable_Feet;
	VRTrackersPawn->bTrackerAvailable_Waist = DataRef.bTrackerAvailable_Waist;
	VRTrackersPawn->bTrackerAvailable_Chest = DataRef.bTrackerAvailable_Chest;
}