// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/ButtonWithText.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UButtonWithText::NativePreConstruct()
{
	Super::NativePreConstruct();
	if (Information) {
		Information->SetText(Info);
		Information->SetColorAndOpacity(TextColor);
	}

	if (BackgroundButton) {
		BackgroundButton->SetBackgroundColor(ButtonColor);
	}
}

void UButtonWithText::NativeConstruct()
{
	Super::NativeConstruct();
	if (BackgroundButton) {
		BackgroundButton->OnClicked.AddDynamic(this, &UButtonWithText::HandleButtonClicked);
	}
}

void UButtonWithText::HandleButtonClicked()
{
	InternalButtonClicked.Broadcast();
}