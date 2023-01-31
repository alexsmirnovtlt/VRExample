// 2021-2022 Alexander Smirnov


#include "UI/Widgets/Settings/UniversalVoiceChatSettingsWidget.h"

#include "Net/VoiceConfig.h"

void UUniversalVoiceChatSettingsWidget::VoiceChat_MicThreshold_Set(float Value)
{
	static IConsoleVariable* MicNoiseGateThresholdCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseGateThreshold"));
	MicNoiseGateThresholdCvar->Set(Value, ECVF_SetByGameSetting);
}

void UUniversalVoiceChatSettingsWidget::VoiceChat_MicInputGain_Set(float Value)
{
	static IConsoleVariable* MicInputGainCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicInputGain"));
	MicInputGainCvar->Set(Value, ECVF_SetByGameSetting);
}

void UUniversalVoiceChatSettingsWidget::VoiceChat_SilenceDetectionThreshold_Set(float Value)
{
	static IConsoleVariable* SilenceDetectionCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionThreshold"));
	SilenceDetectionCVar->Set(Value, ECVF_SetByGameSetting);
}

float UUniversalVoiceChatSettingsWidget::VoiceChat_MicThreshold_Get()
{
	static IConsoleVariable* MicNoiseGateThresholdCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicNoiseGateThreshold"));
	return MicNoiseGateThresholdCvar->GetFloat();
}

float UUniversalVoiceChatSettingsWidget::VoiceChat_MicInputGain_Get()
{
	static IConsoleVariable* MicInputGainCvar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.MicInputGain"));
	return MicInputGainCvar->GetFloat();
}

float UUniversalVoiceChatSettingsWidget::VoiceChat_SilenceDetectionThreshold_Get()
{
	static IConsoleVariable* SilenceDetectionCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("voice.SilenceDetectionThreshold"));
	return SilenceDetectionCVar->GetFloat();
}