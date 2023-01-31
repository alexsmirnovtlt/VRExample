// 2021-2022 Alexander Smirnov


#include "FunctionLibrary/VRFullBodyFunctionLibrary.h"

FString UVRFullBodyFunctionLibrary::AvatarDataStruct_ToString(UPARAM(Ref) const FVRFullBodyAvatarData& AvatarDataStruct)
{
	return AvatarDataStruct.ToString();
}