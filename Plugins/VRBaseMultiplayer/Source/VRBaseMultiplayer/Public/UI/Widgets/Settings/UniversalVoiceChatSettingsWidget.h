// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UniversalVoiceChatSettingsWidget.generated.h"

/**
 * Basic voice chat settings that work with Steam OSS and maybe others
 */
UCLASS()
class VRBASEMULTIPLAYER_API UUniversalVoiceChatSettingsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	void VoiceChat_MicThreshold_Set(float Value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	void VoiceChat_MicInputGain_Set(float Value);
	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	void VoiceChat_SilenceDetectionThreshold_Set(float Value);

	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	float VoiceChat_MicThreshold_Get();
	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	float VoiceChat_MicInputGain_Get();
	UFUNCTION(BlueprintCallable, Category = "Player Controller | Voice Chat")
	float VoiceChat_SilenceDetectionThreshold_Get();
};
