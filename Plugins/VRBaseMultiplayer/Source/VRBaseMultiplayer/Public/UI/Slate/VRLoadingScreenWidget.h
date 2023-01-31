// 2021-2022 Alexander Smirnov

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class VRBASEMULTIPLAYER_API SVRLoadingScreenWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SVRLoadingScreenWidget) {}
	SLATE_END_ARGS()
public:
	void Construct(const FArguments& InArgs);

protected:
	FLinearColor BlackColor = FLinearColor(0.f, 0.f, 0.f, 1.f);
};