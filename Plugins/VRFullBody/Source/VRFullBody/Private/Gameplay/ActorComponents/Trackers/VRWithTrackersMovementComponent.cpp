// 2021-2022 Alexander Smirnov


#include "Gameplay/ActorComponents/Trackers/VRWithTrackersMovementComponent.h"

#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

UVRWithTrackersMovementComponent::UVRWithTrackersMovementComponent()
{
	DisableMovableCamera = true; // Movable camera logic may conflict with overrides for the current class so it better be disabled
}

void UVRWithTrackersMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	// Will be ignoring parent logic (UVRMovableCamMovementComponent) and will be replacing sweep from camera delta to waist movement delta

	if (GetPawnOwner()->IsLocallyControlled())
	{
		FVector WorldWaistDelta = GetWaistWorldDeltaForLastFrame();
		GetPawnOwner()->SetActorLocation(GetActorLocation() + WorldWaistDelta, true);
	}

	Super::Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Character tick is last
}

FVector UVRWithTrackersMovementComponent::GetWaistWorldDeltaForLastFrame()
{
	return GetPawnOwner()->GetActorRotation().RotateVector(GetVRTrackersPawn()->LastWaistLocationDelta);
}

AVRFullBodyPawn_MoreThan3DoF* UVRWithTrackersMovementComponent::GetVRTrackersPawn()
{
	if (VRTrackersPawn) return VRTrackersPawn;
	VRTrackersPawn = Cast<AVRFullBodyPawn_MoreThan3DoF>(GetPawnOwner());
	return VRTrackersPawn;
}