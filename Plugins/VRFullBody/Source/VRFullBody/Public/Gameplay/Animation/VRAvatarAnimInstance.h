// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "VRAvatarAnimInstance.generated.h"

/**
 * AnimInstance that tightly coupled with AFullBodyMannequin
 */
UCLASS()
class VRFULLBODY_API UVRAvatarAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UVRAvatarAnimInstance();
	virtual void NativeInitializeAnimation();
	virtual void NativeUpdateAnimation(float DeltaSeconds);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data")
	bool HasValidVRData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data")
	bool bIsCalibratingTrackers;
	// Valid only for the 3DoF Pawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data")
	float WaistRelativeToCameraHeightOffset;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data")
	class AVRFullBodyPawn* VRFullBodyPawn;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data")
	class AFullBodyAvatar* VRFullBodyAvatar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	FTransform BoneTransform_Head;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	FTransform BoneTransform_Hand_Right;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	FTransform BoneTransform_Hand_Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	bool bIsRotatingInPlace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	bool RotationInPlace_IsClockwise;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Avatar")
	bool bIsMovingTowardsPawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	bool IsMovingWithInput;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector2D LastAvatarMovementDelta;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector2D LastAvatarMovementDelta_Relative;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector2D LastAvatarMovementDelta_Blendspace;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector LastAvatarLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector PawnVelocity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	bool bIsInAir;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Data - Pawn")
	FVector2D MovementBlendspaceAxes;

	virtual void UpdateClassSpecificAnimData(); // atm used for specific setup for this class (3DoF pawn) which a child class (>3DoF pawn - AVRFullBodyPawn_MoreThan3DoF) will not be using

	// Converts world player`s rotation to local 2D vector for movement blendspaces 
	void UpdateMovementAxes(float DeltaSeconds);

	virtual bool GetPawnAndAvatarReferences();
};
