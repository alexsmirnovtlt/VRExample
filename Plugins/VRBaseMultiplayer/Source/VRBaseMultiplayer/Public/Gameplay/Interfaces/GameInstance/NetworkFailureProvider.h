// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NetworkFailureProvider.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UNetworkFailureProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class VRBASEMULTIPLAYER_API INetworkFailureProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | NetworkFailureProvider")
	bool HaveCachedNetworkError();
	virtual bool HaveCachedNetworkError_Implementation() { return false; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | NetworkFailureProvider")
	ENetworkFailure::Type GetAndResetLastClientConnectionError();
	virtual ENetworkFailure::Type GetAndResetLastClientConnectionError_Implementation() { return ENetworkFailure::Type::ConnectionLost; };

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | NetworkFailureProvider")
	FString GetAndResetLastClientConnectionErrorString();
	virtual FString GetAndResetLastClientConnectionErrorString_Implementation() { return ""; };
};
