#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataReceiver.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class USimpleDataReceiver : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that allows to pass objects as base UObjects or send a parameter
 */
class VRBASE_API ISimpleDataReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | SimpleDataReceiver")
	void SendObjects(const TArray<UObject*>& DataArray);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | SimpleDataReceiver")
	void SendParameter(const int32 Parameter);
};
