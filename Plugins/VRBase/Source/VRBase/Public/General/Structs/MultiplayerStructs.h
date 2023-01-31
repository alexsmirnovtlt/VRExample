// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"

#include "Kismet/KismetMathLibrary.h"
#include "UObject/ObjectMacros.h"
//#include "UObject/Object.h"
//#include "UObject/Class.h"
//#include "Engine/NetSerialization.h"

#include "MultiplayerStructs.generated.h"

USTRUCT()
struct VRBASE_API FVRTrackedDeviceData
{
	GENERATED_BODY()

	FVRTrackedDeviceData()
	{
		Location = FVector_NetQuantize10();
		Rotation = FRotator();
	}

	UPROPERTY(Transient)
	FVector_NetQuantize10 Location;
	UPROPERTY(Transient)
	FRotator Rotation; // TODO optimize both
};


USTRUCT()
struct VRBASE_API FVRBasePlayerData
{
	GENERATED_BODY()

	FVRBasePlayerData()
	{
		RightHand = FVRTrackedDeviceData();
		LeftHand = FVRTrackedDeviceData();
		Helm_FloorHeight = 180.f;
		Helm_Rotation = FRotator();
	}

	UPROPERTY(Transient)
	FVRTrackedDeviceData RightHand;
	UPROPERTY(Transient)
	FVRTrackedDeviceData LeftHand;

	UPROPERTY(Transient)
	float Helm_FloorHeight;
	UPROPERTY(Transient)
	FRotator Helm_Rotation;

	// Used for data conversion from local pawn`s to world or simulated to local
	void RotateByYaw(float Yaw)
	{
		if (FMath::Abs(Yaw) <= KINDA_SMALL_NUMBER) return;

		FRotator Offset = FRotator(0, Yaw, 0);

		RightHand.Location = RightHand.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		RightHand.Rotation = UKismetMathLibrary::ComposeRotators(RightHand.Rotation, Offset);

		LeftHand.Location = LeftHand.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		LeftHand.Rotation = UKismetMathLibrary::ComposeRotators(LeftHand.Rotation, Offset);

		Helm_Rotation = UKismetMathLibrary::ComposeRotators(Helm_Rotation, Offset);
	}

	/*bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		return true;
	}*/
};

USTRUCT()
struct VRBASE_API FVRBasePlayerData_AdditTrackers
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FVRTrackedDeviceData RightLeg;
	UPROPERTY(Transient)
	FVRTrackedDeviceData LeftLeg;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Waist;
};

/*
template<>
struct TStructOpsTypeTraits<FVRBasePlayerData> : public TStructOpsTypeTraitsBase2<FVRBasePlayerData>
{
	enum
	{
		WithNetSerializer = true
	};
};*/

USTRUCT(BlueprintType)
struct VRBASE_API FVRFullBodyAvatarData
{
	GENERATED_BODY()

public:
	FVRFullBodyAvatarData()
	{
		AvatarAssetPrimaryID = "";
		AvatarHeight = 170.f;

		AvatarForeheadCameraOffset = 11.f;
		IsUsingAdditionalTrackers = false;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString AvatarAssetPrimaryID;
	// Height is used to properly scale Skeletal Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvatarHeight;
	// Downward distance from Skeletal mesh`s heighest Z value. Used for player`s camera location and head bone affset. Cam should be positioned somewhere between mesh`s eyes
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AvatarForeheadCameraOffset;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// Is using trackers such as Vive Tracker
	bool IsUsingAdditionalTrackers;

	FString ToString() const
	{
		FString TrackersBoolStr;
		FString AssetIDRightPart;
		AvatarAssetPrimaryID.Split(":", &TrackersBoolStr, &AssetIDRightPart);
		TrackersBoolStr = IsUsingAdditionalTrackers ? "1" : "0";

		float Height = int(AvatarHeight * 10) / 10.f;
		float CamOffset = int(AvatarForeheadCameraOffset * 10) / 10.f;

		return 
			AssetIDRightPart + "," + 
			FString::SanitizeFloat(Height) + "," +
			FString::SanitizeFloat(CamOffset) + "," +
			TrackersBoolStr;
	}

	void InitFromString(const FString& Str)
	{
		if (Str.IsEmpty())
		{
			ensure(!Str.IsEmpty());
			return;
		}

		FString AssetIDRightPart;
		FString Height;
		FString CamOffset;
		FString TrackersBoolStr;
		FString TmpStr;

		Str.Split(",", &AssetIDRightPart, &TmpStr);
		TmpStr.Split(",", &Height, &TmpStr);
		TmpStr.Split(",", &CamOffset, &TmpStr);
		TmpStr.Split(",", &TrackersBoolStr, &TmpStr);

		AvatarAssetPrimaryID = "AvatarDataAsset:" + AssetIDRightPart; // TODO Kinda risky to do it like that. But UAvatarDataAsset always returns its id like AvatarDataAsset:something and should always be used for this kind of data so it shouid be fine?
		AvatarHeight = FCString::Atof(*Height);
		AvatarForeheadCameraOffset = FCString::Atof(*CamOffset);
		IsUsingAdditionalTrackers = FCString::Atof(*TrackersBoolStr) > 0.f;
	}

	bool AreValuesValid()
	{
		bool bInvalidData = false;

		bInvalidData |= AvatarHeight < 0;
		bInvalidData |= AvatarForeheadCameraOffset < 0;
		bInvalidData |= AvatarForeheadCameraOffset > AvatarHeight;
		// TODO probably more
		return !bInvalidData;
	}
};

USTRUCT(BlueprintType)
struct VRBASE_API FVRTrackersPlayerData
{
	GENERATED_BODY()

	UPROPERTY(Transient)
	FVRTrackedDeviceData Waist;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Camera;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Chest;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Elbow_Right;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Elbow_Left;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Knee_Right;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Knee_Left;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Foot_Right;
	UPROPERTY(Transient)
	FVRTrackedDeviceData Foot_Left;

	UPROPERTY(Transient) bool bTrackersAvailable_Elbows; // TODO to a single uint8
	UPROPERTY(Transient) bool bTrackersAvailable_Knees;  // TODO and probably optimize possibly empty FVRTrackedDeviceData above
	UPROPERTY(Transient) bool bTrackersAvailable_Feet;
	UPROPERTY(Transient) bool bTrackerAvailable_Waist;
	UPROPERTY(Transient) bool bTrackerAvailable_Chest;

public:
	FVRTrackersPlayerData()
	{
		bTrackersAvailable_Elbows = false;
		bTrackersAvailable_Knees = false;
		bTrackersAvailable_Feet = false;
		bTrackerAvailable_Waist = false;
		bTrackerAvailable_Chest = false;
	}

	void RotateByYaw(float Yaw)
	{
		if (FMath::Abs(Yaw) <= KINDA_SMALL_NUMBER) return;

		FRotator Offset = FRotator(0, Yaw, 0);

		Waist.Location = Waist.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Waist.Rotation = UKismetMathLibrary::ComposeRotators(Waist.Rotation, Offset);

		Camera.Location = Camera.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Camera.Rotation = UKismetMathLibrary::ComposeRotators(Camera.Rotation, Offset);

		Chest.Location = Chest.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Chest.Rotation = UKismetMathLibrary::ComposeRotators(Chest.Rotation, Offset);

		Elbow_Right.Location = Elbow_Right.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Elbow_Right.Rotation = UKismetMathLibrary::ComposeRotators(Elbow_Right.Rotation, Offset);
		Elbow_Left.Location = Elbow_Left.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Elbow_Left.Rotation = UKismetMathLibrary::ComposeRotators(Elbow_Left.Rotation, Offset);

		Knee_Right.Location = Knee_Right.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Knee_Right.Rotation = UKismetMathLibrary::ComposeRotators(Knee_Right.Rotation, Offset);
		Knee_Left.Location = Knee_Left.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Knee_Left.Rotation = UKismetMathLibrary::ComposeRotators(Knee_Left.Rotation, Offset);

		Foot_Right.Location = Foot_Right.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Foot_Right.Rotation = UKismetMathLibrary::ComposeRotators(Foot_Right.Rotation, Offset);
		Foot_Left.Location = Foot_Left.Location.RotateAngleAxis(Yaw, FVector::UpVector);
		Foot_Left.Rotation = UKismetMathLibrary::ComposeRotators(Foot_Left.Rotation, Offset);
	}

	/*bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		return true;
	}*/
};

UENUM(BlueprintType)
enum class ETrackedBone : uint8
{
	Unknown UMETA(DisplayName = "Unknown"),
	Waist UMETA(DisplayName = "Waist"),
	Chest UMETA(DisplayName = "Chest"),
	Elbow_Right UMETA(DisplayName = "Elbow Right"),
	Elbow_Left UMETA(DisplayName = "Elbow Left"),
	Knee_Right UMETA(DisplayName = "Knee Right"),
	Knee_Left UMETA(DisplayName = "Knee Left"),
	Foot_Right UMETA(DisplayName = "Foot Right"),
	Foot_Left UMETA(DisplayName = "Foot Left")
};

UENUM(BlueprintType)
enum class EOnlineSubsystemType : uint8
{
	Undefined UMETA(DisplayName = "Undefined"),
	Null UMETA(DisplayName = "NULL"),
	Steam UMETA(DisplayName = "Steam"),
	EOS UMETA(DisplayName = "EOS"),
	Oculus UMETA(DisplayName = "Oculus")
};

USTRUCT(BlueprintType)
struct VRBASE_API FOnlineSessionSettingsKeyPair
{
	GENERATED_BODY()

	UPROPERTY()
	FName Key;
	UPROPERTY()
	FString Value;
	//EOnlineDataAdvertisementType, defaults to ViaOnlineService
	UPROPERTY()
	uint8 AdvertisementType = 2;
};