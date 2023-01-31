// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Objects/VRControllerState.h"

#include "Kismet/GameplayStatics.h"

#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"

UVRControllerState::UVRControllerState()
{
	bUpdateDistantInteraction = true;
	bUpdateUIInteraction = true;
	bUpdateGrab = true;
}

void UVRControllerState::Setup(AVRControllerBase* NewOwningController, UClass* NewPreviousState)
{
	ensure(NewOwningController);
	OwningHandController = NewOwningController;
	PreviousState = NewPreviousState;
}

AVRControllerBase* UVRControllerState::GetVRController_ForAnotherHand() const
{
	if (!OwningHandController) return nullptr;
	if (auto VRPawn = OwningHandController->GetVRPawn())
		return VRPawn->GetHandController(!OwningHandController->IsRightHand());
	else return nullptr;
}

void UVRControllerState::BackToPreviousState(bool bNotifyOtherController)
{
	if (!ensure(PreviousState && OwningHandController)) return;
	OwningHandController->ChangeState(PreviousState, bNotifyOtherController);
}

AActor* UVRControllerState::SpawnActor(const TSubclassOf<AActor>& ClassToSpawn, ESpawnActorCollisionHandlingMethod SpawnMethod)
{
	if (!ensure(OwningHandController)) return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = SpawnMethod;
	SpawnParams.Owner = OwningHandController->GetVRPawn();

	AActor* SpawnedActor = OwningHandController->GetWorld()->SpawnActor<AActor>(ClassToSpawn, SpawnParams);

	return SpawnedActor;
}

double UVRControllerState::GetWorldDeltaSeconds() const
{
	UWorld* World = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetDeltaSeconds() : 0.0;
}