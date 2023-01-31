// 2021-2022 Alexander Smirnov


#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

#include "IXRTrackingSystem.h"
#include "Net/UnrealNetwork.h"

#include "Gameplay/ActorComponents/Trackers/VRWithTrackersMovementComponent.h"
#include "Gameplay/ActorComponents/Trackers/VRWithTrackersReplicationComp.h"
#include "Gameplay/ActorComponents/Trackers/VRWithTrackersCameraComponent.h"
#include "Gameplay/Actors/VRTrackersFullBodyAvatar.h"
#include "VRFullBody.h"

AVRFullBodyPawn_MoreThan3DoF::AVRFullBodyPawn_MoreThan3DoF(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer
		.SetDefaultSubobjectClass<UVRWithTrackersCameraComponent>("CameraComponent") // Allow camera to only track its location but not move
		.SetDefaultSubobjectClass<UVRWithTrackersReplicationComp>("VRBaseDataReplication") // Include trackers data for replication
		.SetDefaultSubobjectClass<UVRWithTrackersMovementComponent>(ACharacter::CharacterMovementComponentName) // Move pawn with Waist Delta instead of Camera one
	)
{
	FullBodyAvatarClass = AVRTrackersFullBodyAvatar::StaticClass();

	bUpdateAimLocations = false;
	Grip_Right = TEXT("Right"); // Using SteamVR motion source names for everything
	Grip_Left = TEXT("Left");
	
	Tracker_Waist = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Waist"));
	Tracker_Waist->SetupAttachment(FloorLevelComponent);
	Tracker_Waist_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Waist_Offset"));
	Tracker_Waist_Offset->SetupAttachment(Tracker_Waist);

	Tracker_Chest = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Chest"));
	Tracker_Chest->SetupAttachment(FloorLevelComponent);
	Tracker_Chest_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Chest_Offset"));
	Tracker_Chest_Offset->SetupAttachment(Tracker_Chest);

	Tracker_Elbow_Right = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Elbow_Right"));
	Tracker_Elbow_Right->SetupAttachment(FloorLevelComponent);
	Tracker_Elbow_Right_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Elbow_Right_Offset"));
	Tracker_Elbow_Right_Offset->SetupAttachment(Tracker_Elbow_Right);
	Tracker_Elbow_Left = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Elbow_Left"));
	Tracker_Elbow_Left->SetupAttachment(FloorLevelComponent);
	Tracker_Elbow_Left_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Elbow_Left_Offset"));
	Tracker_Elbow_Left_Offset->SetupAttachment(Tracker_Elbow_Left);

	Tracker_Knee_Right = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Knee_Right"));
	Tracker_Knee_Right->SetupAttachment(FloorLevelComponent);
	Tracker_Knee_Right_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Knee_Right_Offset"));
	Tracker_Knee_Right_Offset->SetupAttachment(Tracker_Knee_Right);
	Tracker_Knee_Left = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Knee_Left"));
	Tracker_Knee_Left->SetupAttachment(FloorLevelComponent);
	Tracker_Knee_Left_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Knee_Left_Offset"));
	Tracker_Knee_Left_Offset->SetupAttachment(Tracker_Knee_Left);

	Tracker_Foot_Right = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Foot_Right"));
	Tracker_Foot_Right->SetupAttachment(FloorLevelComponent);
	Tracker_Foot_Right_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Foot_Right_Offset"));
	Tracker_Foot_Right_Offset->SetupAttachment(Tracker_Foot_Right);
	Tracker_Foot_Left = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Foot_Left"));
	Tracker_Foot_Left->SetupAttachment(FloorLevelComponent);
	Tracker_Foot_Left_Offset = CreateDefaultSubobject<USceneComponent>(TEXT("Tracker_Foot_Left_Offset"));
	Tracker_Foot_Left_Offset->SetupAttachment(Tracker_Foot_Left);
}

void AVRFullBodyPawn_MoreThan3DoF::Tick(float DeltaTime)
{
	Super::Super::Tick(DeltaTime);

	if (!bTrackerAvailable_Waist || WaistForwardAxis == EAxis::None) { return; }

	FVector WaistForward;
	if (WaistForwardAxis == EAxis::X) WaistForward = Tracker_Waist_Offset->GetForwardVector();
	else if (WaistForwardAxis == EAxis::Y) WaistForward = Tracker_Waist_Offset->GetRightVector();
	else WaistForward = Tracker_Waist_Offset->GetUpVector();
	// There was a weird situation that getting directly Tracker_Waist_Offset->GetComponentRotation().Yaw returned anything but its real world yaw. No idea why
	float TargetAvatarWorldYaw = WaistForward.ToOrientationQuat().Rotator().Yaw + AdditionalYawAmountPerTick;
	// Kind of the same as the parent but waist instead of a camera
	AvatarRootGimbal->SetWorldRotation(FRotator(0, TargetAvatarWorldYaw, 0));
}

void AVRFullBodyPawn_MoreThan3DoF::UpdateMotionControllersLocations()
{
	Super::UpdateMotionControllersLocations(); // Unlike all parent classes MotionControllers.Num() here is 2!

	if(bEmulateTrackersData)
	{
		if (!HMDEmulatedTransform.Equals(FTransform::Identity)) MainCamera->SetRelativeTransform(HMDEmulatedTransform);
		if (!Hand_Left_EmulatedTransform.Equals(FTransform::Identity)) UpdateControllerLocation(GripLocation_LeftHand, Hand_Left_EmulatedTransform.GetLocation(), Hand_Left_EmulatedTransform.GetRotation().Rotator());
		if (!Hand_Right_EmulatedTransform.Equals(FTransform::Identity)) UpdateControllerLocation(GripLocation_RightHand, Hand_Right_EmulatedTransform.GetLocation(), Hand_Right_EmulatedTransform.GetRotation().Rotator());
	}

	IXRTrackingSystem* TrackingSys = GEngine->XRSystem.Get();
	if (!TrackingSys || !bTrackerAvailable_Waist) return; // Its pointless to do a fullbody with trackers if the important Waist Tracker is not present. Anim blueprint should handle this situation as if its a Parent Fullbody pawn with only HMD and hands

	FVector OutLocation;
	FQuat OutQuat;

	for (auto& KeyValue : TrackedBones)
	{
		if (bEmulateTrackersData) { if (!GetEmulatedTrackerData(KeyValue.Value, OutQuat, OutLocation)) continue; }
		else if (!TrackingSys->GetCurrentPose(KeyValue.Value, OutQuat, OutLocation)) continue;

		if (KeyValue.Key == ETrackedBone::Waist)
		{
			if (!bDiscardFirstWaistLocation)
			{
				LastWaistLocationDelta = OutLocation - LastWaistLocation;
				LastWaistLocationDelta.Z = 0;
				LastWaistLocation = OutLocation;

				Tracker_Waist->SetRelativeLocationAndRotation(FVector(0, 0, OutLocation.Z), OutQuat);
			}
			else bDiscardFirstWaistLocation = false;
		}
		else if (KeyValue.Key == ETrackedBone::Chest)
			UpdateControllerLocation(Tracker_Chest, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Elbow_Right)
			UpdateControllerLocation(Tracker_Elbow_Right, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Elbow_Left)
			UpdateControllerLocation(Tracker_Elbow_Left, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Knee_Right)
			UpdateControllerLocation(Tracker_Knee_Right, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Knee_Left)
			UpdateControllerLocation(Tracker_Knee_Left, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Foot_Right)
			UpdateControllerLocation(Tracker_Foot_Right, OutLocation, OutQuat.Rotator());
		else if (KeyValue.Key == ETrackedBone::Foot_Left)
			UpdateControllerLocation(Tracker_Foot_Left, OutLocation, OutQuat.Rotator());
	}
}

void AVRFullBodyPawn_MoreThan3DoF::UpdateControllerLocation(USceneComponent* UpdatingComponent, const FVector& NewLocation, const FRotator& NewRotation)
{
	FVector RelativeToWaistLocation = NewLocation - LastWaistLocation;
	UpdatingComponent->SetRelativeLocationAndRotation(Tracker_Waist->GetRelativeLocation() + RelativeToWaistLocation, NewRotation);
}

UVRWithTrackersMovementComponent* AVRFullBodyPawn_MoreThan3DoF::GetVRTrackersMovementComponent()
{
	if (IsValid(VRTrackersMovementComponent)) return VRTrackersMovementComponent;
	VRTrackersMovementComponent = Cast<UVRWithTrackersMovementComponent>(GetMovementComponent());
	return VRTrackersMovementComponent;
}

UVRWithTrackersReplicationComp* AVRFullBodyPawn_MoreThan3DoF::GetVRTrackersReplicationComponent()
{
	if (IsValid(VRTrackersDataReplicationComponent)) return VRTrackersDataReplicationComponent;
	VRTrackersDataReplicationComponent = Cast<UVRWithTrackersReplicationComp>(VRBaseDataReplicationComponent);
	return VRTrackersDataReplicationComponent;
}

void AVRFullBodyPawn_MoreThan3DoF::ReinitTrackedBonesMap(const TMap<ETrackedBone, int32>& NewMap)
{
	TrackedBones = NewMap;

	TArray<ETrackedBone> TrackersInUse;
	for (auto& KeyValue : TrackedBones) TrackersInUse.Add(KeyValue.Key);

	bTrackerAvailable_Waist = TrackersInUse.Contains(ETrackedBone::Waist);
	bTrackerAvailable_Chest = TrackersInUse.Contains(ETrackedBone::Chest);
	bTrackersAvailable_Elbows = TrackersInUse.Contains(ETrackedBone::Elbow_Left) && TrackersInUse.Contains(ETrackedBone::Elbow_Right);
	bTrackersAvailable_Feet = TrackersInUse.Contains(ETrackedBone::Foot_Left) && TrackersInUse.Contains(ETrackedBone::Foot_Right);
	bTrackersAvailable_Knees = bTrackersAvailable_Feet && // Its assumed that tracking knees without feet is pointless and user needs to reattach those trackers to their feet instead
		TrackersInUse.Contains(ETrackedBone::Knee_Left) && TrackersInUse.Contains(ETrackedBone::Knee_Right);
}

void AVRFullBodyPawn_MoreThan3DoF::ReplicateVRDataToServer(float DeltaTime)
{
	if (auto TrackersReplicationComp = GetVRTrackersReplicationComponent())
	{
		auto DataToReplicate = FVRBasePlayerData();
		if (TrackersReplicationComp->GetLocalDataToReplicate(DeltaTime, DataToReplicate))
			Server_UpdateVRBaseData(DataToReplicate);

		auto TrackersDataToReplicate = FVRTrackersPlayerData();
		if (TrackersReplicationComp->GetTrackersDataToReplicate(DeltaTime, TrackersDataToReplicate))
			Server_UpdateVRTrackersData(TrackersDataToReplicate);
	}
}

void AVRFullBodyPawn_MoreThan3DoF::Server_UpdateVRTrackersData_Implementation(const FVRTrackersPlayerData& NewReplicatedTrackersData)
{
	ReplicatedTrackersData = MoveTemp(const_cast<FVRTrackersPlayerData&>(NewReplicatedTrackersData));
	OnRep_VRTrackersData();
}

void AVRFullBodyPawn_MoreThan3DoF::OnRep_VRTrackersData()
{
	if (auto TrackersReplicationComp = GetVRTrackersReplicationComponent())
		TrackersReplicationComp->SetNewTrackersData(ReplicatedTrackersData);
}

void AVRFullBodyPawn_MoreThan3DoF::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AVRFullBodyPawn_MoreThan3DoF, ReplicatedTrackersData, COND_SimulatedOnly);
}

#pragma region DEBUG_TrackersEmulation

bool AVRFullBodyPawn_MoreThan3DoF::GetEmulatedTrackerData(int32 DeviceID, FQuat& CurrentOrientation, FVector& CurrentPosition)
{
	if (!EmulatedTrackerTransforms.Contains(DeviceID)) return false;
	const auto FoundTransform = EmulatedTrackerTransforms.Find(DeviceID);
	CurrentPosition = FoundTransform->GetLocation();
	CurrentOrientation = FoundTransform->GetRotation();
	return true;
}

TMap<int32, FTransform> AVRFullBodyPawn_MoreThan3DoF::GetActiveTrackersMap(bool bWorldLocation)
{
	if (bEmulateTrackersData) return EmulatedTrackerTransforms;

	TMap<int32, FTransform> OutMap;
	TArray<int32> DeviceIds;
	IXRTrackingSystem* TrackingSys = GEngine->XRSystem.Get();
	if (!TrackingSys) return OutMap;
	
	TrackingSys->EnumerateTrackedDevices(DeviceIds, EXRTrackedDeviceType::Other);
	for(const auto& ID : DeviceIds)
	{
		if (ID == 0) { UE_LOG(VRFullbodyLog, Warning, TEXT("%s Device with id 0 was enumerated!"), *FString(__FUNCTION__)); continue; } // May be useless but just in case

		FVector Location;
		FQuat Rotation;
		if (TrackingSys->GetCurrentPose(ID, Rotation, Location))
		{
			if (bWorldLocation)
			{
				auto TrackingToWorld = TrackingSys->GetTrackingToWorldTransform();
				OutMap.Add(ID, FTransform(
					TrackingToWorld.TransformRotation(Rotation),
					TrackingToWorld.TransformPosition(Location)
				));
			}
			else OutMap.Add(ID, FTransform(Rotation, Location));
		}
	}

	return OutMap;
}

#pragma endregion