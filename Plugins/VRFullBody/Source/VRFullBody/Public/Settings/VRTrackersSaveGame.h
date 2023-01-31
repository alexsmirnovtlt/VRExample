// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "VRTrackersSaveGame.generated.h"

class AVRFullBodyPawn_MoreThan3DoF;
enum class ETrackedBone : uint8;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRTrackersSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Waist_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Chest_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Elbow_Right_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Elbow_Left_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Knee_Right_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Knee_Left_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Foot_Right_Offset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTransform Tracker_Foot_Left_Offset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<ETrackedBone, int32> TrackedBonesMap;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Save Game")
	static const FString& GetVRTrackersDataSaveSlotName() { return SlotName; }
	static const FString SlotName;

	UFUNCTION(BlueprintCallable)
	void ApplyOffsetsFromSave(class AVRFullBodyPawn_MoreThan3DoF* Pawn);
};
