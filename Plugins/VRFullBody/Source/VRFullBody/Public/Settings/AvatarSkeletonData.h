// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AvatarSkeletonData.generated.h"

/**
 * Specific values for Player Avatar per skeleton
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class VRFULLBODY_API UAvatarSkeletonData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Waist;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Chest;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Shoulder_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Shoulder_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Elbow_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Elbow_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Hand_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Hand_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Knee_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Knee_Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Foot_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Bone Names")
	FName Foot_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Debug")
	TArray<FString> Visualization_BoneNamePartsToIgnore;

	// Character with trackers`s forward rotation will match this axis combined with UAvatarDataAsset::AvatarGimbalAdditionalYawOffset 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Axes")
	TEnumAsByte<EAxis::Type> WaistForwardAxis;

	UFUNCTION(BlueprintCallable)
	static const TArray<FName> GetBoneNamesAsArray(TSubclassOf<UAvatarSkeletonData> AssetClass)
	{
		TArray<FName> Out;
		if (auto CDO = AssetClass.GetDefaultObject())
		{
			if (!CDO->Head.IsNone()) Out.Add(CDO->Head);
			if (!CDO->Waist.IsNone()) Out.Add(CDO->Waist);
			if (!CDO->Chest.IsNone()) Out.Add(CDO->Chest);
			if (!CDO->Shoulder_Right.IsNone()) Out.Add(CDO->Elbow_Right);
			if (!CDO->Shoulder_Left.IsNone()) Out.Add(CDO->Elbow_Left);
			if (!CDO->Elbow_Right.IsNone()) Out.Add(CDO->Elbow_Right);
			if (!CDO->Elbow_Left.IsNone()) Out.Add(CDO->Elbow_Left);
			if (!CDO->Hand_Right.IsNone()) Out.Add(CDO->Hand_Right);
			if (!CDO->Hand_Left.IsNone()) Out.Add(CDO->Hand_Left);
			if (!CDO->Knee_Right.IsNone()) Out.Add(CDO->Knee_Right);
			if (!CDO->Knee_Left.IsNone()) Out.Add(CDO->Knee_Left);
			if (!CDO->Foot_Right.IsNone()) Out.Add(CDO->Foot_Right);
			if (!CDO->Foot_Left.IsNone()) Out.Add(CDO->Foot_Left);	
		}
		return Out;
	}
};
