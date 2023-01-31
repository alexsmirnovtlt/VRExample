// 2021-2022 Alexander Smirnov


#include "Gameplay/ActorComponents/Trackers/VRWithTrackersCameraComponent.h"

#include "Rendering/MotionVectorSimulation.h"
#include "IXRTrackingSystem.h"
#include "IXRCamera.h"

#include "Gameplay/Player/Pawns/VRFullBodyPawn_MoreThan3DoF.h"

void UVRWithTrackersCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	// Copy of a parent function, with additions in one place
	// Just sets vars for pawn to use

	if (!GetVRTrackersPawn()) { Super::GetCameraView(DeltaTime, DesiredView); return; }

	if (GEngine && GEngine->XRSystem.IsValid() && GetWorld() && GetWorld()->WorldType != EWorldType::Editor)
	{
		IXRTrackingSystem* XRSystem = GEngine->XRSystem.Get();
		auto XRCamera = XRSystem->GetXRCamera();

		if (XRCamera.IsValid())
		{
			if (XRSystem->IsHeadTrackingAllowedForWorld(*GetWorld()))
			{
				const FTransform ParentWorld = CalcNewComponentToWorld(FTransform());

				XRCamera->SetupLateUpdate(ParentWorld, this, bLockToHmd == 0);

				if (bLockToHmd)
				{
					FQuat Orientation;
					FVector Position;
					if (XRCamera->UpdatePlayerCamera(Orientation, Position))
					{
						// The only place with changes

						LastHMDMovementDelta = FVector2D(Position - LastHMDLocation);
						LastHMDLocation = Position; // Both lines need to be present to be compatible with VRBase pawn logic that relies on those vars

						FVector RelativeToWaistLocation = LastHMDLocation - Pawn->LastWaistLocation;
						SetRelativeLocationAndRotation(Pawn->Tracker_Waist->GetRelativeLocation() + RelativeToWaistLocation, Orientation.Rotator());

						//
					}
					else
					{
						ResetRelativeTransform();
					}
				}

				XRCamera->OverrideFOV(this->FieldOfView);
			}
		}
	}

	if (bUsePawnControlRotation)
	{
		const APawn* OwningPawn = Cast<APawn>(GetOwner());
		const AController* OwningController = OwningPawn ? OwningPawn->GetController() : nullptr;
		if (OwningController && OwningController->IsLocalPlayerController())
		{
			const FRotator PawnViewRotation = OwningPawn->GetViewRotation();
			if (!PawnViewRotation.Equals(GetComponentRotation()))
			{
				SetWorldRotation(PawnViewRotation);
			}
		}
	}

	if (bUseAdditiveOffset)
	{
		FTransform OffsetCamToBaseCam = AdditiveOffset;
		FTransform BaseCamToWorld = GetComponentToWorld();
		FTransform OffsetCamToWorld = OffsetCamToBaseCam * BaseCamToWorld;

		DesiredView.Location = OffsetCamToWorld.GetLocation();
		DesiredView.Rotation = OffsetCamToWorld.Rotator();
	}
	else
	{
		DesiredView.Location = GetComponentLocation();
		DesiredView.Rotation = GetComponentRotation();
	}

	DesiredView.FOV = bUseAdditiveOffset ? (FieldOfView + AdditiveFOVOffset) : FieldOfView;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;

	// See if the CameraActor wants to override the PostProcess settings used.
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}

	// If this camera component has a motion vector simumlation transform, use that for the current view's previous transform
	DesiredView.PreviousViewTransform = FMotionVectorSimulation::Get().GetPreviousTransform(this);
}

AVRFullBodyPawn_MoreThan3DoF* UVRWithTrackersCameraComponent::GetVRTrackersPawn()
{
	if (IsValid(Pawn)) return Pawn;
	Pawn = Cast<AVRFullBodyPawn_MoreThan3DoF>(GetOwner());
	return Pawn;
}