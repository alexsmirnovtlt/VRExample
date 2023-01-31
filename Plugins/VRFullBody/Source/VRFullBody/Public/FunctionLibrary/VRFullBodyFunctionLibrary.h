// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "General/Structs/MultiplayerStructs.h"
#include "VRFullBodyFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class VRFULLBODY_API UVRFullBodyFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContext"), Category = "VR Full Body Function Library")
	static FString AvatarDataStruct_ToString(UPARAM(Ref) const FVRFullBodyAvatarData& AvatarDataStruct);
};
