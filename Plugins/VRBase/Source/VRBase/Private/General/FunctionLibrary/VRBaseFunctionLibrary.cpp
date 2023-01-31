// 2021-2022 Alexander Smirnov


#include "General/FunctionLibrary/VRBaseFunctionLibrary.h"

#include "GameMapsSettings.h"
#include "IXRTrackingSystem.h"

void UVRBaseFunctionLibrary::GetDetailedWorldInfo(bool& IsEditor, bool& IsPIE, bool& IsShipping, UObject* WorldContext)
{
	IsEditor = false;
	IsPIE = false;
	IsShipping = false;

	if (!WorldContext || !WorldContext->GetWorld()) return;

	IsPIE = WorldContext->GetWorld()->WorldType == EWorldType::PIE;

#if WITH_EDITOR
	IsEditor = true;
#endif
#if UE_BUILD_SHIPPING
	IsShipping = true;
#endif
}
/* TODO Move somewhere else and add "EngineSettings" to Build.cs
FString UVRBaseFunctionLibrary::GetDefaultGameMap()
{
	return UGameMapsSettings::GetGameDefaultMap();
}

FName UVRBaseFunctionLibrary::GetHMDSystemName()
{
	if (GEngine->XRSystem.IsValid())
		return GEngine->XRSystem->GetSystemName();
	else return FName(TEXT("Unknown"));
}*/