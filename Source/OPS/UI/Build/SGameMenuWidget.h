// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SGameMenuWidget.generated.h"

class USGenericButton;
class USBuildMenuWidget;
/**
 * 
 */
UCLASS(Abstract)
class ORTHOPVPSTRATEGY_API USGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (BindWidget))
	USGenericButton* BuildButton;

	UPROPERTY(meta = (BindWidget))
	USBuildMenuWidget* BuildMenuWidget;

protected:
	UFUNCTION()
	void ToggleBuildMenu();

	UFUNCTION()
	void OnBuildItemSelected();

	UPROPERTY()
	bool bBuildMenuOpen = false;
	
};
