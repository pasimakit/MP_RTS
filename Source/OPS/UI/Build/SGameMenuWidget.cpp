// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameMenuWidget.h"
#include "OPS/UI/Build/SGenericButton.h"
#include "OPS/UI/Build/SBuildMenuWidget.h"
#include "Components/Button.h"

void USGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BuildButton)
	{
		BuildButton->Button->OnClicked.AddDynamic(this, &USGameMenuWidget::ToggleBuildMenu);
		if (BuildMenuWidget)
		{
			BuildMenuWidget->SetVisibility(ESlateVisibility::Hidden);
			bBuildMenuOpen = false;
			BuildMenuWidget->OnBuildItemSelectedEvent.AddDynamic(this, &USGameMenuWidget::OnBuildItemSelected);
		}
	}
}

void USGameMenuWidget::ToggleBuildMenu()
{
	bBuildMenuOpen = !bBuildMenuOpen;

	if (BuildMenuWidget)
	{
		BuildMenuWidget->SetVisibility(bBuildMenuOpen ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
		bBuildMenuOpen ? BuildMenuWidget->DisplayBuildMenu() : BuildMenuWidget->EndDisplayBuildMenu();
	}
}

void USGameMenuWidget::OnBuildItemSelected()
{
	ToggleBuildMenu();
}
