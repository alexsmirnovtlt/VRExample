// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogic_Arc_Rotatable.h"

#include "General/Gameplay/Actors/VRControllerBase.h"


void UVRTeleportLogic_Arc_Rotatable::UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation)
{
	Super::UpdateTeleportArc(HorizontalInput, VerticalInput, StartLocation, StartRotation);

	if (IsValid(TeleportDestinationActor))
	{
		if (auto VRController = ControllerState->GetVRController())
		{
			if (auto AimComponent = VRController->GetAimComponent())
			{
				auto ControllerWorldYaw = AimComponent->GetComponentRotation().Yaw;

				auto InputLocalRotation = FVector(-HorizontalInput, VerticalInput, 0.f).Rotation();
				auto InputWorldRotation = FRotator(0.f, InputLocalRotation.Yaw - 90.f + ControllerWorldYaw, 0.f);
				TeleportDestinationActor->SetActorRotation(InputWorldRotation);
			}
		}
	}
}

void UVRTeleportLogic_Arc_Rotatable::GetTeleportDestination(bool& bIsValidLocation, FVector& Location, FRotator& Rotation)
{
	bIsValidLocation = IsValid(TeleportDestinationActor) && !TeleportDestinationActor->IsHidden();
	if (bIsValidLocation)
	{
		Location = TeleportDestinationActor->GetActorLocation();
		Rotation = TeleportDestinationActor->GetActorRotation();
	}
}