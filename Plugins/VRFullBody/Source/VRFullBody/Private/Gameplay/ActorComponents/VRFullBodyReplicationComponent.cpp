// 2021-2022 Alexander Smirnov


#include "Gameplay/ActorComponents/VRFullBodyReplicationComponent.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"

bool UVRFullBodyReplicationComponent::GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef)
{
	if (!GetVRFullBodyPawn()) { ensure(false); return false; }

	if (OwnerVRFullBodyPawn->IsCameraDetached()) return false; // TODO Probably not a good logic decision, parent class does the same 

	LastTransformsUpdateTime += DeltaTime; // Limit client to only a number of server RPCs per second
	if (LastTransformsUpdateTime < 1.f / MaxServerRPCsPerSecond) return false;
	LastTransformsUpdateTime -= 1.f / MaxServerRPCsPerSecond;

	// ReportGripLocationForHands bool gets ignored unlike the parent, because it should always be true for fullbody pawns (no detached physical hands)

	FVector Location;
	FRotator Rotation;

	CombineRelative(OwnerVRFullBodyPawn->GetGripComponent(false), OwnerVRFullBodyPawn->LocalPlayerBone_Hand_Left, Location, Rotation);
	DataRef.LeftHand.Location = Location;
	DataRef.LeftHand.Rotation = Rotation;
	CombineRelative(OwnerVRFullBodyPawn->GetGripComponent(true), OwnerVRFullBodyPawn->LocalPlayerBone_Hand_Right, Location, Rotation);
	DataRef.RightHand.Location = Location;
	DataRef.RightHand.Rotation = Rotation;

	DataRef.Helm_FloorHeight =
		OwnerVRFullBodyPawn->GetCamera()->GetRelativeLocation().Z + OwnerVRFullBodyPawn->LocalPlayerHeadBone->GetRelativeLocation().Z;
	DataRef.Helm_Rotation =
		OwnerVRFullBodyPawn->GetCamera()->GetRelativeRotation();

	if (ConvertDataToWorldSpace) DataRef.RotateByYaw(OwnerVRFullBodyPawn->GetActorRotation().Yaw);
	
	return true;
}

AVRFullBodyPawn* UVRFullBodyReplicationComponent::GetVRFullBodyPawn()
{
	if (!OwnerVRPawn) return nullptr;
	OwnerVRFullBodyPawn = Cast<AVRFullBodyPawn>(OwnerVRPawn);
	return OwnerVRFullBodyPawn;
}