// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Animation/VRAvatarAnimInstance.h"
#include "AvatarWithTrackersAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UAvatarWithTrackersAnimInstance : public UVRAvatarAnimInstance
{
	GENERATED_BODY()

protected:
	virtual void UpdateClassSpecificAnimData() override; // override 3DoF pawn specific calculations 
	virtual bool GetPawnAndAvatarReferences();
	class AVRFullBodyPawn_MoreThan3DoF* TrackersPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Waist;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Chest;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Elbow_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Elbow_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Knee_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Knee_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Foot_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers")
	FTransform BoneTransform_Foot_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers") bool bTrackersAvailable_Elbows;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers") bool bTrackersAvailable_Knees;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers") bool bTrackersAvailable_Feet;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers") bool bTrackerAvailable_Waist;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar Trackers") bool bTrackerAvailable_Chest;
};
