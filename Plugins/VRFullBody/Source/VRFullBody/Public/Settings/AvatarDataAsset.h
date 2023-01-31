// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Settings/AvatarSkeletonData.h"
#include "AvatarDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UAvatarDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (AssetBundles = "Mesh"))
	class USkeletalMesh* Mesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (AssetBundles = "Mesh"))
	float MeshHeight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (AssetBundles = "Mesh"))
	class UAnimBlueprintGeneratedClass* AnimInstance; // TODO Not really a specific subclass but it works, unlike editor-only UAnimBlueprint*;
	// Specific values for Player Avatar bone offsets. Varies per skeleton
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (AssetBundles = "Mesh"))
	class TSubclassOf<UAvatarSkeletonData> SkeletonData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Avatar", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;

	// Offset to place an avatar a little behind the pawn so camera would be just inbetween the eyes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	FVector2D MeshOffset;
	// Location and Rotation of the Avatar Root (or Root bone located on a Floor) to the Head Bone on a T-Pose. Used to set affector for the Head. Will be the same across assets with the same skeleton.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	FTransform HeadBoneTPoseTransform;
	// Default Offset (loc+rot) from controller location to the Right Hand bone. Used as affector. Will be the same across assets with the same skeleton.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	FTransform RightHandToBoneOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	FTransform LeftHandToBoneOffset;
	// Avatar`s Skeleton might be rotated differently then the default one so it needs to be offset. Or height might also needs to be offset. Its advised to change only Loc:Z and Rot:Yaw
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	FTransform AvatarRootOffset;
	// Trackers pawn will offset its forward rotation by that amount to compensate for waist`s original bone rotation. 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Offsets")
	float AvatarGimbalAdditionalYawOffset;
};
