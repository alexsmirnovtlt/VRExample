// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VoipTalkerStatusReceiver.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UVoipTalkerStatusReceiver : public UInterface
{
	GENERATED_BODY()
};

class VRBASEMULTIPLAYER_API IVoipTalkerStatusReceiver
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Interfaces | VoipTalkerStatusReceiver")
	bool OnTalkingStateChanged(bool bIsTalking);
};
