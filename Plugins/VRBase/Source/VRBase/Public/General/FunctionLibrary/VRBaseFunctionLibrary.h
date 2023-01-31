// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VRBaseFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VRBASE_API UVRBaseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// IsEditor: is non packaged build (WindowsEditor), PIE or standalone
	// IsPIE: is run by pressing Play button from the Editor
	// IsShipping: Is WindowsNoEditor Shipping build
	// Examples:
	// !IsEditor && !IsShipping - WindowsNoEditor Development build
	// IsEditor && !IsPIE - WindowsEditor Standalone game
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"), Category = "VR Base Function Library")
	static void GetDetailedWorldInfo(bool& IsEditor, bool& IsPIE, bool& IsShipping, UObject* WorldContext);

	/*UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base Function Library")
	static FString GetDefaultGameMap();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "VR Base Function Library")
	static FName GetHMDSystemName();*/
};
