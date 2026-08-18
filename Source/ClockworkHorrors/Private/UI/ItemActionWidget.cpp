// Copyright Aluminati Studios Publishing 2026. All Rights Reserved.


#include "UI/ItemActionWidget.h"
#include "Components/WidgetSwitcher.h"
#include "UI/ButtonWithText.h"

UItemActionWidget::UItemActionWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsFocusable = true;
}

void UItemActionWidget::NativePreConstruct()
{
}

void UItemActionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EquipSwitcher)
	{
		EquipSwitcher->SetActiveWidgetIndex(0);
	}

	if(EquipButton)
	{
		EquipButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnEquipActionButtonClicked);
	}
	if (UnequipButton)
	{
		UnequipButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnUnequipActionButtonClicked);
	}

	if(UseButton)
	{
		UseButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnUseActionButtonClicked);
	}

	if (DropButton)
	{
		DropButton->InternalButtonClicked.AddDynamic(this, &UItemActionWidget::OnDropActionButtonClicked);
	}
}

void UItemActionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if(!IsVisible())
	{
		return;
	}
}

void UItemActionWidget::ShowWidget()
{
	SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::ShowEquipAction()
{
	if (!EquipButton)
	{
		return;
	}

	EquipButton->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::ShowUseAction()
{
	if(!UseButton)
	{
		return;
	}

	UseButton->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::ShowDropAction()
{
	if(!DropButton)
	{
		return;
	}

	DropButton->SetVisibility(ESlateVisibility::Visible);
}

void UItemActionWidget::HideWidget()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::HideEquipAction()
{
	if(!EquipButton)
	{
		return;
	}

	EquipButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::HideUseAction()
{
	if(!UseButton)
	{
		return;
	}

	UseButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::HideDropAction()
{
	if(!DropButton)
	{
		return;
	}

	DropButton->SetVisibility(ESlateVisibility::Collapsed);
}

void UItemActionWidget::ResetMenu()
{
	EquipSwitcher->SetActiveWidgetIndex(0);
	ShowEquipAction();
	ShowUseAction();
	ShowDropAction();
}

void UItemActionWidget::OnEquipActionButtonClicked()
{
	if(!EquipSwitcher && !EquipButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(0);

	HideWidget();
}

void UItemActionWidget::OnUnequipActionButtonClicked()
{
	if(!EquipSwitcher && !UnequipButton)
	{
		return;
	}
	
	OnItemActionClicked.Broadcast(1);

	HideWidget();
}

void UItemActionWidget::OnUseActionButtonClicked()
{
	if(!UseButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(2);

	HideWidget();
}

void UItemActionWidget::OnDropActionButtonClicked()
{
	if(!DropButton)
	{
		return;
	}

	OnItemActionClicked.Broadcast(3);

	HideWidget();
}
