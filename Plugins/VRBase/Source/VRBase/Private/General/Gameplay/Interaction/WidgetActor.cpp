// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Interaction/WidgetActor.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Components/WidgetComponent.h"
#include "NiagaraComponent.h"

#include "General/Interfaces/DataReceiver.h"
#include "General/Gameplay/Actors/VRControllerBase.h"
#include "General/Gameplay/Interaction/Player/DistantInteractionComponent.h"

void AWidgetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed || EndPlayReason == EEndPlayReason::RemovedFromWorld)
		if(ActiveHand) ResetInteraction(ActiveHand);

	Super::EndPlay(EndPlayReason);
}

void AWidgetActor::StartUITrace(AVRControllerBase* Hand, const FHitResult& HitResult)
{
	if (!ensure(Hand)) return;
	CacheNiagraData(Hand);
	if (CachedNiagraComponent)
	{
		UpdateUITrace(Hand, HitResult);
		CachedNiagraComponent->SetHiddenInGame(false);
	}
}

void AWidgetActor::UpdateUITrace(AVRControllerBase* Hand, const FHitResult& HitResult)
{
	if (!ensure(Hand)) return;

	if (CachedNiagraComponent)
	{
		TArray<FVector> PointsArray;
		PointsArray.Add(HitResult.TraceStart);
		PointsArray.Add(HitResult.Location);

		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(CachedNiagraComponent, CachedNiagraParameterName, PointsArray);
	}
	else StartUITrace(Hand, HitResult); // At some rare cases this update might happen without a call to StartUITrace()
}

void AWidgetActor::EndUITrace(AVRControllerBase* Hand)
{
	if (!ensure(Hand)) return;
	if (CachedNiagraComponent)
	{
		CachedNiagraComponent->SetHiddenInGame(true);
		CachedNiagraComponent = nullptr;
	}
}

void AWidgetActor::CacheNiagraData(AVRControllerBase* Hand)
{
	if (!ensure(Hand)) return;
	CachedNiagraComponent = Hand->GetNiagaraSystem(HandNiagaraParticleType::UIInteraction);
	CachedNiagraParameterName = Hand->GetNiagaraParameterName(HandNiagaraParticleType::UIInteraction);
}

void AWidgetActor::ResetInteraction(AVRControllerBase* Hand)
{
	if(Hand) Hand->GetDistantInteractionComponent()->InvalidateInteractableObject();
}

void AWidgetActor::ClearSlateWidgetReferenceForHand(AVRControllerBase* Hand)
{
	if (Hand) Hand->GetDistantInteractionComponent()->ClearSlateWidgetReference();
}

void AWidgetActor::SendIntegerToAWidget(const UWidgetComponent* WidgetComponent, int32 Value)
{
	if (auto Widget = WidgetComponent->GetWidget())
	{
		if (Widget->Implements<USimpleDataReceiver>())
			ISimpleDataReceiver::Execute_SendParameter(Widget, Value);
	}
}

void AWidgetActor::SendObjectsToAWidget(const UWidgetComponent* WidgetComponent, const TArray<UObject*>& DataArray)
{
	if (auto Widget = WidgetComponent->GetWidget())
	{
		if (Widget->Implements<USimpleDataReceiver>())
			ISimpleDataReceiver::Execute_SendObjects(Widget, DataArray);
	}
}