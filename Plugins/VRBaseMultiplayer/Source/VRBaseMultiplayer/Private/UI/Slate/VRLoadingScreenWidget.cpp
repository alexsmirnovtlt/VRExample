// 2021-2022 Alexander Smirnov


#include "UI/Slate/VRLoadingScreenWidget.h"

#include "SlateOptMacros.h"
#include "Widgets/Images/SImage.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SVRLoadingScreenWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
		.VAlign(VAlign_Fill)
		.HAlign(HAlign_Fill)
		[
			SNew(SImage)
			.ColorAndOpacity(BlackColor)
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION