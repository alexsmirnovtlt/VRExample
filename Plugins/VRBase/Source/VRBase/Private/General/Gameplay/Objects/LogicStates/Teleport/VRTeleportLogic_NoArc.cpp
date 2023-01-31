// Fill out your copyright notice in the Description page of Project Settings.


#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogic_NoArc.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"

void UVRTeleportLogic_NoArc::Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter)
{
	Super::Activate(OwningControllerState, NewTeleportTraceComponent, NiagraVectorTraceParameter);

	NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	QueryParams = FCollisionQueryParams();
	QueryParams.bReturnPhysicalMaterial = false;
	QueryParams.bTraceComplex = TeleportTraceComplex;

	ObjectQueryParams = FCollisionObjectQueryParams(TraceAgainstObjectTypes);
}

void UVRTeleportLogic_NoArc::UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation)
{
	FHitResult HitResult;
	const FVector EndLocation = StartLocation + StartRotation.Vector() * TraceLength;

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		StartLocation,
		EndLocation,
		ObjectQueryParams,
		QueryParams
	);

	const TArray<FVector> TraceLocations = { StartLocation, EndLocation };
	VisualizeTeleportTrace(HitResult, TraceLocations);
}

void UVRTeleportLogic_NoArc::VisualizeTeleportTrace(const FHitResult& HitInfo, const TArray<FVector> TraceLocations)
{
	if (TeleportTraceComponent)
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportTraceComponent, NiagraVectorArrayParameter, TraceLocations);

	bool bIsValidNavmeshLocation = false;
	FVector ProjectedLocation;

	if (HitInfo.IsValidBlockingHit())
	{
		FNavLocation NavLocationStruct;
		bIsValidNavmeshLocation = NavigationSystem->ProjectPointToNavigation(HitInfo.Location, NavLocationStruct, NavMeshCheckExtent);
		if (bIsValidNavmeshLocation) ProjectedLocation = NavLocationStruct.Location + TeleportDestinationLocationOffset;
	}
	if (IsValid(TeleportDestinationActor))
	{
		TeleportDestinationActor->SetActorHiddenInGame(!bIsValidNavmeshLocation);
		if (bIsValidNavmeshLocation) TeleportDestinationActor->SetActorLocation(ProjectedLocation);
	}

	if (bHideTraceOnInvalidDestination && TeleportTraceComponent)
		TeleportTraceComponent->SetHiddenInGame(!bIsValidNavmeshLocation);
}