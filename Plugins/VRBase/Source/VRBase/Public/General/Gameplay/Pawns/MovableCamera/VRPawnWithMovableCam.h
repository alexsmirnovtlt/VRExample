// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"
#include "VRPawnWithMovableCam.generated.h"

/**
 * VR Pawn with different collision handling.
 * Player that is colliding with a wall or other obstacle can move towards it with its cam only without collision restrictions.
 * The pawn itself will still honor collisions, "detaching" player from its pawn (by changing relative Cam`s location). 
 * Provides more interactive freedom for VR player at a cost of increasing logic complexity and more complex egde-cases.
 */
UCLASS()
class VRBASE_API AVRPawnWithMovableCam : public AVRPawnBase
{
	GENERATED_BODY()

public:
	AVRPawnWithMovableCam(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	void BeginPlay() override;

public:
	virtual bool IsCameraDetached() const override;

protected:
	class UVRMovableCamMovementComponent* MovableCamMovementComponent;
};
