// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EventSubscriptable.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FExecutableEventDelegate, FString, Option, UObject*, DataObject);

UINTERFACE(MinimalAPI, BlueprintType)
class UEventSubscriptable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface to forward general type events between uobjects without being bounded to specific BP classes
 * (f.e. spawn actor with widget component and subscribe to widget`s button directly)
 */
class VRBASE_API IEventSubscriptable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | IEventSubscriptable")
	void SubscribeToEvent(const FString& Parameter, const FExecutableEventDelegate& DelegateRef);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interfaces | IEventSubscriptable")
	void UnsubscribeFromEvent(const FString& Parameter, const FExecutableEventDelegate& DelegateRef);
};
