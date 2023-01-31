// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/ActorComponents/VRFullBodyReplicationComponent.h"
#include "General/Structs/MultiplayerStructs.h"
#include "VRWithTrackersReplicationComp.generated.h"

class AVRFullBodyPawn_MoreThan3DoF;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRWithTrackersReplicationComp final : public UVRFullBodyReplicationComponent
{
	GENERATED_BODY()

public:
	UVRWithTrackersReplicationComp();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	virtual bool GetLocalDataToReplicate(float DeltaTime, FVRBasePlayerData& DataRef) override;
	
	bool GetTrackersDataToReplicate(float DeltaTime, FVRTrackersPlayerData& DataRef);
	void SetNewTrackersData(FVRTrackersPlayerData& DataRef);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Network variables")
	float MaxTrackersServerRPCsPerSecond;

	AVRFullBodyPawn_MoreThan3DoF* GetVRTrackersPawn();
	UPROPERTY()
	AVRFullBodyPawn_MoreThan3DoF* VRTrackersPawn;

	float LastTransformsUpdateTime_Trackers;
	bool bShouldLerp_Trackers;

	FTransform Transform_Waist;
	FTransform Transform_Camera;
	FTransform Transform_Chest;
	FTransform Transform_Elbow_Right;
	FTransform Transform_Elbow_Left;
	FTransform Transform_Knee_Right;
	FTransform Transform_Knee_Left;
	FTransform Transform_Foot_Right;
	FTransform Transform_Foot_Left;

	FTransform LastTransform_Waist;
	FTransform LastTransform_Camera;
	FTransform LastTransform_Chest;
	FTransform LastTransform_Elbow_Right;
	FTransform LastTransform_Elbow_Left;
	FTransform LastTransform_Knee_Right;
	FTransform LastTransform_Knee_Left;
	FTransform LastTransform_Foot_Right;
	FTransform LastTransform_Foot_Left;
};
