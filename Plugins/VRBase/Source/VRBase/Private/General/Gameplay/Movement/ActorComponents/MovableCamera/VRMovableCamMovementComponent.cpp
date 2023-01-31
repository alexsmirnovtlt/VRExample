// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Movement/ActorComponents/MovableCamera/VRMovableCamMovementComponent.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"

void UVRMovableCamMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (DisableMovableCamera)
	{
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		return;
	}

	if (GetVRBasePawn() && VRBasePawn->IsLocallyControlled())
	{
		auto FloorComp = VRBasePawn->GetFloorLevelComponent();
		FVector WorldHMDMovementDelta = GetHMDWorldDeltaForLastFrame();
		FVector FloorRelativeLocation_Initial = FloorComp->GetRelativeLocation();
		bDetachedCam = !FVector2D(FloorRelativeLocation_Initial).IsZero();

		if (bDetachedCam)
		{
			ConsumeInputVector();

			// First, find desired FloorComp (the one with the Camera) location by adding HMDDelta to it 
			// Second, try to move the pawn closer to that desired location (FloorComp`s location is unimportant right now, its got cached)
			// Third, set FloorComp location to the desired one
			// Fourth, check detachment state, because we can again be attached to the pawn (if Cam location == Pawn location)
			// Fifth, reattach if needed (zero Relative location of FloorComp)

			FVector DesiredCameraWorldLocation = FloorComp->GetComponentLocation() + WorldHMDMovementDelta;
			FVector DesiredPawnWorldLocation = DetachedCam_GetPawnSweepLocation(
				VRBasePawn->GetActorLocation(),
				DesiredCameraWorldLocation
			);
			VRBasePawn->SetActorLocation(DesiredPawnWorldLocation, true);
			
			if (FVector2D(VRBasePawn->GetActorLocation()).Equals(FVector2D(DesiredCameraWorldLocation)))
			{
				SetCameraDetachedState(false); // Reattach
				FloorComp-> SetRelativeLocation(FVector(0.f, 0.f, FloorRelativeLocation_Initial.Z));
			}
			else FloorComp->SetWorldLocation(DesiredCameraWorldLocation); // Actual third step, can be placed before the if condition 
		}
		else
		{
			FVector TargetPawnLocation = GetActorLocation() + WorldHMDMovementDelta;
			VRBasePawn->SetActorLocation(TargetPawnLocation, true);
			if (!VRBasePawn->GetActorLocation().Equals(TargetPawnLocation))
			{
				SetCameraDetachedState(true); // Detach
				FVector FloorRelativeLocation = TargetPawnLocation - VRBasePawn->GetActorLocation();
				FloorRelativeLocation.Z = FloorRelativeLocation_Initial.Z;
				FloorComp->SetRelativeLocation(FloorRelativeLocation);
			}
		}
	}

	Super::Super::TickComponent(DeltaTime, TickType, ThisTickFunction); // Calling Character Movement Component
}

FVector UVRMovableCamMovementComponent::DetachedCam_GetPawnSweepLocation(const FVector& PawnLocation, const FVector& TargetCamLocation)
{
	FVector UnclampedMoveDirection = TargetCamLocation - PawnLocation;
	FVector2D ClampedMoveDirection = FVector2D(UnclampedMoveDirection);

	// TODO probably change the logic, correctly limit resulting vector by length and/or X and Y values
	// Now its fixed amount of centimeters per frame and its sucks 
	if (GetNetMode() != ENetMode::NM_Standalone && MaxPawnSweepCMLengthPerFrame > 0.f)
		ClampedMoveDirection = FVector2D(UnclampedMoveDirection.GetClampedToMaxSize2D(MaxPawnSweepCMLengthPerFrame));
	//

	ReplicatedHMDDelta = ClampedMoveDirection;

	return PawnLocation + FVector(ClampedMoveDirection.X, ClampedMoveDirection.Y, 0.f);
}

void UVRMovableCamMovementComponent::SetCameraDetachedState(bool bDetached)
{
	if (DisableMovableCamera) return; // Super is doing nothing

	if(bDetached) ConsumeInputVector();

	VRBasePawn->OnCameraDetachStateChanged(bDetached);
	VRBasePawn->GetVRMovementComponent()->SetCanMove(!bDetached, !bDetached);
}

FVector2D UVRMovableCamMovementComponent::GetHMDWorldDeltaForReplication()
{
	if (DisableMovableCamera) return Super::GetHMDWorldDeltaForReplication();

	// At this point the data that will be send to the server may differ from actual auth client data because his Cam may be detached
	// In this case actual pawn sweep direction will not be the same as the result of GetHMDWorldDeltaForLastFrame() but rather a custom value that is determined at this own TickComponent()
	if (bDetachedCam) return ReplicatedHMDDelta;
	else return FVector2D(GetHMDWorldDeltaForLastFrame());
}