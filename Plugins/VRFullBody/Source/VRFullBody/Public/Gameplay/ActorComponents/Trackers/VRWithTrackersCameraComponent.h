// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "VRWithTrackersCameraComponent.generated.h"

class AVRFullBodyPawn_MoreThan3DoF;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRWithTrackersCameraComponent final : public UVRCameraComponent
{
	GENERATED_BODY()

public:
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	FORCEINLINE AVRFullBodyPawn_MoreThan3DoF* GetVRTrackersPawn();
	AVRFullBodyPawn_MoreThan3DoF* Pawn;
};
