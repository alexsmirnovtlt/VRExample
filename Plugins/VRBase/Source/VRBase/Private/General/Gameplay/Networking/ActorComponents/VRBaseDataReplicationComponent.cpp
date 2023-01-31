// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Networking/ActorComponents/VRBaseDataReplicationComponent.h"

#include "Kismet/KismetMathLibrary.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"

// TODO Optimize structs serialization

UVRBaseDataReplicationComponent::UVRBaseDataReplicationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	ConvertDataToWorldSpace = true;

	Transform_Hand_Right = FTransform(FTransform::Identity);
	Transform_Hand_Left = FTransform(FTransform::Identity);
	Transform_HMD = FTransform(FQuat::Identity, FVector(0, 0, 184.f));

	DetachableCamera = false;

	MaxServerRPCsPerSecond = 10.f;
	ReportGripLocationForHands = true;
	
	LastTransformsUpdateTime = 0.f;
	bShouldLerp = false;
}

void UVRBaseDataReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerVRPawn = Cast<AVRPawnBase>(GetOwner());
	if(!ensure(OwnerVRPawn)) Deactivate();
}

void UVRBaseDataReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Tick is enabled only for Simulated Proxies

	if (!bShouldLerp) return;

	LastTransformsUpdateTime += DeltaTime;
	float LerpModifier = LastTransformsUpdateTime / (1.f / MaxServerRPCsPerSecond);
	if (LastTransformsUpdateTime >= 1.f)
	{
		LerpModifier = 1.f;
		bShouldLerp = false;
	}

	if (!Transform_Hand_Right.GetLocation().IsNearlyZero())
		LerpRelativeTransform(OwnerVRPawn->GetGripComponent(true), LastTransform_Hand_Right, Transform_Hand_Right, LerpModifier);
	if (!Transform_Hand_Left.GetLocation().IsNearlyZero())
		LerpRelativeTransform(OwnerVRPawn->GetGripComponent(false), LastTransform_Hand_Left, Transform_Hand_Left, LerpModifier);
	
	LerpRelativeTransform(OwnerVRPawn->GetCamera(), LastTransform_HMD, Transform_HMD, LerpModifier);
}

bool UVRBaseDataReplicationComponent::GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef)
{
	if (!OwnerVRPawn) { ensure(false); return false; }
	
	if (OwnerVRPawn->IsCameraDetached()) return false; // TODO Probably not a good logic decision
	// TODO If Edited then edit UVRFullBodyReplicationComponent::GetLocalDataToReplicate too!

	LastTransformsUpdateTime += DeltaTime; // Limit client to only a number of server RPCs per second
	if (LastTransformsUpdateTime < 1.f / MaxServerRPCsPerSecond) return false;
	LastTransformsUpdateTime -= 1.f / MaxServerRPCsPerSecond;

	if (ReportGripLocationForHands)
	{ // Kinematic hands will always be at grip`s locations
		DataRef.LeftHand.Location = OwnerVRPawn->GetGripComponent(false)->GetRelativeLocation();
		DataRef.LeftHand.Rotation = OwnerVRPawn->GetGripComponent(false)->GetRelativeRotation();

		DataRef.RightHand.Location = OwnerVRPawn->GetGripComponent(true)->GetRelativeLocation();
		DataRef.RightHand.Rotation = OwnerVRPawn->GetGripComponent(true)->GetRelativeRotation();
	}
	else
	{ // Physical hands (f.e using Phys constraints) may be anywhere
		FTransform RelativeTransform = OwnerVRPawn->GetHandController(false)->GetTransform().GetRelativeTransform(OwnerVRPawn->GetTransform());
		DataRef.LeftHand.Location = RelativeTransform.GetLocation();
		DataRef.LeftHand.Rotation = RelativeTransform.GetRotation().Rotator();
		// TODO Check if it is working alright
		RelativeTransform = OwnerVRPawn->GetHandController(true)->GetTransform().GetRelativeTransform(OwnerVRPawn->GetTransform());
		DataRef.RightHand.Location = RelativeTransform.GetLocation();
		DataRef.RightHand.Rotation = RelativeTransform.GetRotation().Rotator();
	}

	DataRef.Helm_FloorHeight = OwnerVRPawn->GetCamera()->GetRelativeLocation().Z;
	DataRef.Helm_Rotation = OwnerVRPawn->GetCamera()->GetRelativeRotation();

	if (ConvertDataToWorldSpace) DataRef.RotateByYaw(OwnerVRPawn->GetActorRotation().Yaw);

	return true;
}

void UVRBaseDataReplicationComponent::SetNewVRData(FVRBasePlayerData& NewVRData)
{
	if (!OwnerVRPawn) return;

	if (ConvertDataToWorldSpace) NewVRData.RotateByYaw(-OwnerVRPawn->GetActorRotation().Yaw);

	LastTransformsUpdateTime = 0.f;
	bShouldLerp = true;
	// Set lerp from
	LastTransform_Hand_Right = OwnerVRPawn->GetGripComponent(true)->GetRelativeTransform();
	LastTransform_Hand_Left = OwnerVRPawn->GetGripComponent(false)->GetRelativeTransform();
	LastTransform_HMD = OwnerVRPawn->GetCamera()->GetRelativeTransform();
	// Set lerp to
	Transform_Hand_Right = FTransform(NewVRData.RightHand.Rotation, NewVRData.RightHand.Location, LastTransform_Hand_Right.GetScale3D());
	Transform_Hand_Left = FTransform(NewVRData.LeftHand.Rotation, NewVRData.LeftHand.Location, LastTransform_Hand_Left.GetScale3D());
	Transform_HMD = FTransform(NewVRData.Helm_Rotation, FVector(0.f, 0.f, NewVRData.Helm_FloorHeight), LastTransform_HMD.GetScale3D());
}