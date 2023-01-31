// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"
#include "General/Structs/MultiplayerStructs.h"
#include "VRFullBodyPawn_MoreThan3DoF.generated.h"

class UVRWithTrackersMovementComponent;
class UVRWithTrackersReplicationComp;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API AVRFullBodyPawn_MoreThan3DoF final : public AVRFullBodyPawn
{
	GENERATED_BODY()

	friend class UVRWithTrackersMovementComponent;
	friend class UAvatarWithTrackersAnimInstance;
	friend class UVRWithTrackersReplicationComp;
	friend class UVRWithTrackersCameraComponent;
	friend class UVRTrackersSaveGame; // it applies trackers offset from there
	
public:
	AVRFullBodyPawn_MoreThan3DoF(const FObjectInitializer& ObjectInitializer);
	virtual void Tick(float DeltaTime) override;

public:
	UVRWithTrackersMovementComponent* GetVRTrackersMovementComponent();
	UVRWithTrackersReplicationComp* GetVRTrackersReplicationComponent();
	UFUNCTION(BlueprintCallable)
	void ReinitTrackedBonesMap(const TMap<ETrackedBone, int32>& NewMap);

protected:
	virtual void ReplicateVRDataToServer(float DeltaTime) override;
	virtual void UpdateMotionControllersLocations() override;
	void UpdateControllerLocation(USceneComponent* UpdatingComponent, const FVector& NewLocation, const FRotator& NewRotation) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Waist;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Waist_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Chest;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Chest_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Elbow_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Elbow_Right_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Elbow_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Elbow_Left_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Knee_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Knee_Right_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Knee_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Knee_Left_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Foot_Right;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Foot_Right_Offset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Foot_Left;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Tracker_Foot_Left_Offset;

	UPROPERTY(BlueprintReadOnly) bool bTrackersAvailable_Elbows = false;
	UPROPERTY(BlueprintReadOnly) bool bTrackersAvailable_Knees = false;
	UPROPERTY(BlueprintReadOnly) bool bTrackersAvailable_Feet = false;
	UPROPERTY(BlueprintReadOnly) bool bTrackerAvailable_Waist = false;
	UPROPERTY(BlueprintReadOnly) bool bTrackerAvailable_Chest = false;

	bool bDiscardFirstWaistLocation = true; // To not teleport for possibly quite some distance on the first frame
	FVector LastWaistLocation;
	FVector LastWaistLocationDelta;

	UVRWithTrackersMovementComponent* VRTrackersMovementComponent;
	UVRWithTrackersReplicationComp* VRTrackersDataReplicationComponent;

	UPROPERTY(BlueprintReadOnly)
	TMap<ETrackedBone, int32> TrackedBones;

	// Networking 
protected:
	UFUNCTION(Server, WithValidation, Unreliable)
	void Server_UpdateVRTrackersData(const FVRTrackersPlayerData& NewReplicatedTrackersData);
	bool Server_UpdateVRTrackersData_Validate(const FVRTrackersPlayerData& NewReplicatedTrackersData) { return true; };

	UPROPERTY(ReplicatedUsing = OnRep_VRTrackersData)
	FVRTrackersPlayerData ReplicatedTrackersData;
	UFUNCTION()
	void OnRep_VRTrackersData();
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Trackers emulation logic mostly for testing. Fill the map with transforms and call ReinitTrackedBonesMap() with corresponding IDs
protected:
	bool GetEmulatedTrackerData(int32 DeviceID, FQuat& CurrentOrientation, FVector& CurrentPosition);
	UFUNCTION(BlueprintCallable)
	TMap<int32, FTransform> GetActiveTrackersMap(bool bWorldLocation = false);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers Emulation")
	bool bEmulateTrackersData = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers Emulation", meta = (EditCondition = "bEmulateTrackersData"))
	TMap<int32, FTransform> EmulatedTrackerTransforms;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers Emulation", meta = (EditCondition = "bEmulateTrackersData"))
	FTransform HMDEmulatedTransform = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers Emulation", meta = (EditCondition = "bEmulateTrackersData"))
	FTransform Hand_Left_EmulatedTransform = FTransform::Identity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Trackers Emulation", meta = (EditCondition = "bEmulateTrackersData"))
	FTransform Hand_Right_EmulatedTransform = FTransform::Identity;
};
