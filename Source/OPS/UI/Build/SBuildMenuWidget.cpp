// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuildMenuWidget.h"
#include "Engine/AssetManager.h"
#include "Components/ListView.h"
#include "Components/Border.h"
#include "Kismet/GameplayStatics.h"
#include "OPS/Controller/SPlayerController.h"
#include "OPS/Build/SBuildComponent.h"
#include "OPS/Data/SBuildItemDataAsset.h"

void USBuildMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MenuBorder)
	{
		MenuBorder->SetVisibility(ESlateVisibility::Hidden);
	}
}

void USBuildMenuWidget::DisplayBuildMenu()
{
	if (!BuildItemsList) return;
	
	verify((AssetManager = UAssetManager::GetIfInitialized()) != nullptr);
	verify((SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);
	verify((BuildComponent = USBuildComponent::FindBuildComponent(SPlayerController)) != nullptr);

	BuildItemsList->ClearListItems();
	
	// Populate build menu
	const TArray<FPrimaryAssetId> BuildItemDataAssets = BuildComponent->GetBuildData();

	for (int i = 0; i < BuildItemDataAssets.Num(); ++i)
	{
		if (USBuildItemDataAsset* BuildData = Cast<USBuildItemDataAsset>(AssetManager->GetPrimaryAssetObject(BuildItemDataAssets[i])))
		{
			BuildItemsList->AddItem(BuildData);
		}
	}

	BuildItemsList->SetVisibility(ESlateVisibility::Visible);
	MenuBorder->SetVisibility(ESlateVisibility::Visible);

	if (BuildComponent && !BuildComponent->OnBuildModeEnterEvent.IsBound())
	{
		BuildComponent->OnBuildModeEnterEvent.AddDynamic(this, &USBuildMenuWidget::OnBuildModeEnteredEvent);
	}
}

void USBuildMenuWidget::EndDisplayBuildMenu() const
{
	BuildItemsList->ClearListItems();
	BuildItemsList->SetVisibility(ESlateVisibility::Hidden);
	MenuBorder->SetVisibility(ESlateVisibility::Hidden);

	if (BuildComponent && BuildComponent->OnBuildModeEnterEvent.IsBound())
	{
		BuildComponent->OnBuildModeEnterEvent.RemoveDynamic(this, &USBuildMenuWidget::OnBuildModeEnteredEvent);
	}
}

void USBuildMenuWidget::OnBuildModeEnteredEvent()
{
	EndDisplayBuildMenu();
}
