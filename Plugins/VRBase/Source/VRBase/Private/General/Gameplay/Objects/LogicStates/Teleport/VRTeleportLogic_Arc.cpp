// 2021-2022 Alexander Smirnov


#include "General/Gameplay/Objects/LogicStates/Teleport/VRTeleportLogic_Arc.h"

#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/AssetManager.h"
#include "NiagaraComponent.h"
#include "NavigationSystem.h"

// TODO Skip frames? UpdateTeleportArc gets executed every frame

void UVRTeleportLogic_Arc::Activate(UVRControllerState* OwningControllerState, UNiagaraComponent* NewTeleportTraceComponent, FName NiagraVectorTraceParameter)
{
	Super::Activate(OwningControllerState, NewTeleportTraceComponent, NiagraVectorTraceParameter);

	NavigationSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());

	ProjectileParams = FPredictProjectilePathParams(TeleportProjectileRadius, FVector(), FVector(), TeleportMaxSimulationTime);
	ProjectileParams.bTraceWithCollision = true;
	ProjectileParams.bTraceComplex = TeleportTraceComplex;
	ProjectileParams.ActorsToIgnore = ActorsToIgnoreWhileTrace;
	ProjectileParams.DrawDebugType = bShowDebugTrace ? EDrawDebugTrace::ForOneFrame : EDrawDebugTrace::None;
	ProjectileParams.DrawDebugTime = .1f;
	ProjectileParams.SimFrequency = TeleportMaxSimulationFrequency;
	//Params.OverrideGravityZ = OverrideGravityZ;
	ProjectileParams.ObjectTypes = TraceAgainstObjectTypes;
	ProjectileParams.bTraceWithChannel = false;
}

void UVRTeleportLogic_Arc::UpdateTeleportArc(float HorizontalInput, float VerticalInput, FVector StartLocation, FRotator StartRotation)
{
	ProjectileParams.StartLocation = StartLocation;
	ProjectileParams.LaunchVelocity = StartRotation.Vector() * TeleportProjectileSpeed;

	FPredictProjectilePathResult PredictResult;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, ProjectileParams, PredictResult);

	FHitResult HitResult = PredictResult.HitResult;
	FVector OutLastTraceDestination = PredictResult.LastTraceDestination.Location;
	TArray<FVector> OutPathPositions;
	OutPathPositions.Empty(PredictResult.PathData.Num());
	for (const FPredictProjectilePathPointData& PathPoint : PredictResult.PathData)
		OutPathPositions.Add(PathPoint.Location);
	
	OutPathPositions.Insert(StartLocation, 0);
	VisualizeTeleportTrace(HitResult, OutPathPositions);
}

void UVRTeleportLogic_Arc::VisualizeTeleportTrace(const FHitResult& HitInfo, const TArray<FVector> TraceLocations)
{
	// Updating trace path with Niagra if able
	if (TeleportTraceComponent)
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(TeleportTraceComponent, NiagraVectorArrayParameter, TraceLocations);

	bool bIsValidNavmeshLocation = false;
	FVector ProjectedLocation;

	if (HitInfo.IsValidBlockingHit() && NavigationSystem)
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
}