// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Notifiable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UNotifiable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Version of other similar interfaces: EventSubscriptable and DataReceiver but with different parameters
 */
class VRBASE_API INotifiable
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | Notifiable")
	void ReceiveNotification_NoParams();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | Notifiable")
	void ReceiveNotification_String(const FString& String);
};
