// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Movement/ActorComponents/MovableCamera/VRMovableCamMovementComponent.h"
#include "VRWithTrackersMovementComponent.generated.h"

class AVRFullBodyPawn_MoreThan3DoF;

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRWithTrackersMovementComponent final : public UVRMovableCamMovementComponent
{
	GENERATED_BODY()

public:
	UVRWithTrackersMovementComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	FVector GetWaistWorldDeltaForLastFrame();
	virtual FVector2D GetHMDWorldDeltaForReplication() { return FVector2D(GetWaistWorldDeltaForLastFrame()); };

protected:
	FORCEINLINE AVRFullBodyPawn_MoreThan3DoF* GetVRTrackersPawn();
	AVRFullBodyPawn_MoreThan3DoF* VRTrackersPawn;
};
