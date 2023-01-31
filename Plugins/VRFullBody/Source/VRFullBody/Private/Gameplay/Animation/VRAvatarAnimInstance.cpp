// 2021-2022 Alexander Smirnov


#include "Gameplay/Animation/VRAvatarAnimInstance.h"

#include "General/Gameplay/Movement/ActorComponents/VRBaseMovementComponent.h"
#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "Gameplay/Player/Pawns/VRFullBodyPawn.h"
#include "Gameplay/Actors/FullBodyAvatar.h"

UVRAvatarAnimInstance::UVRAvatarAnimInstance()
{
	HasValidVRData = false;
	LastAvatarLocation = FVector(0);
}

void UVRAvatarAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	GetPawnAndAvatarReferences();
}

void UVRAvatarAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!GetPawnAndAvatarReferences()) return;
	if (VRFullBodyPawn->IsCameraDetached()) return;

	if (VRFullBodyPawn->IsLocallyControlled())
	{
		BoneTransform_Hand_Right = VRFullBodyPawn->LocalPlayerBone_Hand_Right->GetComponentTransform();
		BoneTransform_Hand_Left = VRFullBodyPawn->LocalPlayerBone_Hand_Left->GetComponentTransform();
	}
	else
	{
		BoneTransform_Hand_Right = VRFullBodyPawn->GripLocation_RightHand->GetComponentTransform();
		BoneTransform_Hand_Left = VRFullBodyPawn->GripLocation_LeftHand->GetComponentTransform();
	}

	BoneTransform_Head = VRFullBodyPawn->LocalPlayerHeadBone->GetComponentTransform();

	PawnVelocity = VRFullBodyPawn->GetVelocity();

	IsMovingWithInput = !PawnVelocity.IsNearlyZero();
	bIsMovingTowardsPawn = VRFullBodyAvatar->bIsMovingTowardsPawn;
	bIsInAir = VRFullBodyPawn->GetVRMovementComponent()->MovementMode == EMovementMode::MOVE_Falling;
	bIsCalibratingTrackers = VRFullBodyAvatar->bIsInCalibrationState;
	bIsRotatingInPlace = VRFullBodyAvatar->bIsRotatingTowardsCamera && !VRFullBodyAvatar->bIsMovingTowardsPawn;
	RotationInPlace_IsClockwise = VRFullBodyAvatar->bRotatingTowardsCamera_Clockwise;

	LastAvatarMovementDelta = FVector2D(LastAvatarLocation);
	LastAvatarLocation = VRFullBodyAvatar->GetActorLocation();
	
	LastAvatarMovementDelta = FVector2D(LastAvatarLocation) - LastAvatarMovementDelta;
	LastAvatarMovementDelta_Relative = LastAvatarMovementDelta.GetRotated(-VRFullBodyAvatar->GetActorRotation().Yaw);
	LastAvatarMovementDelta_Blendspace = FVector2D(LastAvatarMovementDelta_Relative.Y, LastAvatarMovementDelta_Relative.X);

	UpdateMovementAxes(DeltaSeconds);
	UpdateClassSpecificAnimData();
}

void UVRAvatarAnimInstance::UpdateClassSpecificAnimData()
{
	// Used for waist Z offset
	WaistRelativeToCameraHeightOffset = 
		VRFullBodyAvatar->AvatarData.AvatarHeight - VRFullBodyAvatar->AvatarData.AvatarForeheadCameraOffset - // Default Cam height
		VRFullBodyPawn->GetCamera()->GetRelativeLocation().Z; // Actual Cam height
}

bool UVRAvatarAnimInstance::GetPawnAndAvatarReferences()
{
	if (VRFullBodyAvatar && VRFullBodyPawn) return true;

	if (auto Mesh = GetSkelMeshComponent())
	{
		if (auto Actor = Mesh->GetOwner())
		{
			VRFullBodyAvatar = Cast<AFullBodyAvatar>(Actor);
			if (VRFullBodyAvatar) VRFullBodyPawn = VRFullBodyAvatar->GetFullBodyPawn();
			HasValidVRData = VRFullBodyAvatar && VRFullBodyPawn;
			return HasValidVRData;
		}
	}

	return false;
}

void UVRAvatarAnimInstance::UpdateMovementAxes(float DeltaSeconds)
{
	FVector2D PlayerWorldVelocity;

	if (IsMovingWithInput) PlayerWorldVelocity = FVector2D(PawnVelocity);
	else
	{
		PlayerWorldVelocity = LastAvatarMovementDelta / DeltaSeconds; // TODO probably incorrect
		float MaxSpeed = VRFullBodyPawn->GetVRMovementComponent()->GetMaxSpeed(); // TODO probably should use Sprint speed
		PlayerWorldVelocity = PlayerWorldVelocity.ClampAxes(-MaxSpeed, MaxSpeed);
	}

	// TODO
	// GetRotated should be different:
	// - non trackers avatar case should rotate by its avatar (which is rotated by pawn`s gimble which is rotated by camera)
	// - trackers avatar case can rotate aroung waist bone world location  

	MovementBlendspaceAxes = PlayerWorldVelocity.GetRotated(-VRFullBodyAvatar->GetActorRotation().Yaw);
	MovementBlendspaceAxes = FVector2D(MovementBlendspaceAxes.Y, MovementBlendspaceAxes.X);

	/*if (!MovementBlendspaceAxes.IsNearlyZero())
	{
		GEngine->AddOnScreenDebugMessage(0, DeltaSeconds, FColor::Red, 
			FString::SanitizeFloat(MovementBlendspaceAxes.X) + "   " + 
			FString::SanitizeFloat(MovementBlendspaceAxes.Y) + "  " + 
			FString::SanitizeFloat(VRFullBodyAvatar->GetActorRotation().Yaw)
		);

		//UE_LOG(LogTemp, Warning, TEXT("12345 UpdateMovementAxes, input: %d, %f, %f"), IsMovingWithInput ? 1 : 0, MovementBlendspaceAxes.X, MovementBlendspaceAxes.Y);
	}*/
}