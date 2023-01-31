#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogicBase.h"

#include "NiagaraComponent.h"
#include "Engine/AssetManager.h"

#include "General/Gameplay/Movement/ActorComponents/VRCameraComponent.h"
#include "General/Gameplay/Objects/VRControllerState.h"
#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Gameplay/Pawns/VRPawnBase.h"

void UVRTeleportLogicBase::Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter)
{
	ControllerState = OwningControllerState;
	NiagraVectorArrayParameter = NiagraVectorTraceParameter;

	if (NewTeleportTraceComponent)
	{
		TeleportTraceComponent = NewTeleportTraceComponent;
		TeleportTraceComponent->SetHiddenInGame(false);
	}

	auto AssetManager = UAssetManager::GetIfValid();
	if (AssetManager && TeleportDestinationAsset.ToSoftObjectPath().IsValid())
	{
		FStreamableManager& StreamableManager = AssetManager->GetStreamableManager();
		TeleportDestinationHandle = StreamableManager.RequestAsyncLoad(
			TeleportDestinationAsset.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &UVRTeleportLogicBase::OnTeleportDestinationLoaded)
		);
	}
}

void UVRTeleportLogicBase::Deactivate()
{
	ControllerState = nullptr;
	if (TeleportTraceComponent) TeleportTraceComponent->SetHiddenInGame(true);
	if (TeleportDestinationHandle.IsValid()) TeleportDestinationHandle.Get()->ReleaseHandle();
	if (IsValid(TeleportDestinationActor)) TeleportDestinationActor->Destroy();
}

void UVRTeleportLogicBase::OnTeleportDestinationLoaded()
{
	if (!TeleportDestinationHandle.IsValid()) return;
	if (TeleportDestinationHandle.Get()->HasLoadCompleted())
	{
		auto TeleportArrowLoadedClass = Cast<UClass>(TeleportDestinationHandle.Get()->GetLoadedAsset());
		TeleportDestinationActor = GetWorld()->SpawnActor<AActor>(TeleportArrowLoadedClass);
	}

	TeleportDestinationHandle.Reset(); // TODO check correct GC for handle and actor
}

void UVRTeleportLogicBase::GetTeleportDestination(bool& bIsValidLocation, FVector& Location, FRotator& Rotation)
{
	bIsValidLocation = IsValid(TeleportDestinationActor) && !TeleportDestinationActor->IsHidden();
	if (bIsValidLocation)
	{
		Location = TeleportDestinationActor->GetActorLocation();

		if (ControllerState)
		{
			if (auto VRController = ControllerState->GetVRController())
			{
				if (auto VRPawn = VRController->GetVRPawn())
				{
					Rotation = VRPawn->GetCamera()->GetComponentRotation();
				}
			}
		}
		else Rotation = TeleportDestinationActor->GetActorRotation();
	}
}