// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Pawns/MovableCamera/VRPawnWithMovableCam.h"

#include "General/Gameplay/Movement/ActorComponents/MovableCamera/VRMovableCamMovementComponent.h"

AVRPawnWithMovableCam::AVRPawnWithMovableCam(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UVRMovableCamMovementComponent>(ACharacter::CharacterMovementComponentName))
{

}

void AVRPawnWithMovableCam::BeginPlay()
{
	Super::BeginPlay();
	MovableCamMovementComponent = Cast<UVRMovableCamMovementComponent>(GetMovementComponent());
}

bool AVRPawnWithMovableCam::IsCameraDetached() const
{
	return MovableCamMovementComponent->IsCameraDetached();
}