// Fill out your copyright notice in the Description page of Project Settings.


#include "SBuildItemWidget.h"
#include "OPS/UI/Build/SGenericButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "OPS/Data/SBuildItemDataAsset.h"
#include "OPS/Controller/SPlayerController.h"
#include "OPS/Build/SBuildComponent.h"
#include "Kismet/GameplayStatics.h"

void USBuildItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();


	if (PlaceBuildButton)
	{
		PlaceBuildButton->Button->OnClicked.AddDynamic(this, &USBuildItemWidget::OnPlaceBuildItemSelected);
	}
	verify((SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);
	BuildComponent = USBuildComponent::FindBuildComponent(SPlayerController);
}

void USBuildItemWidget::OnPlaceBuildItemSelected()
{
	if (!BuildComponent) return;

	BuildComponent->EnterBuildPlacementMode(Data);
}

void USBuildItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	
	Data = Cast<USBuildItemDataAsset>(ListItemObject);

	if (Data)
	{
		TitleText->SetText(Data->TitleText);
		Image->SetBrushFromTexture(Data->Image.LoadSynchronous());
	}
}
