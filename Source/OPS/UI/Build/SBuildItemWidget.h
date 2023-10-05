// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "SBuildItemWidget.generated.h"

class USGenericButton;
class USBuildItemDataAsset;
class USBuildComponent;
class ASPlayerController;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS(Abstract)
class ORTHOPVPSTRATEGY_API USBuildItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	USGenericButton* PlaceBuildButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TitleText;

	UPROPERTY(meta = (BindWidget))
	UImage* Image;

protected:
	UFUNCTION()
	void OnPlaceBuildItemSelected();

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UPROPERTY()
	USBuildItemDataAsset* Data;

	UPROPERTY()
	ASPlayerController* SPlayerController;

	UPROPERTY()
	USBuildComponent* BuildComponent;
};
