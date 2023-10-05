// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SBuildMenuWidget.generated.h"


class UListView;
class UBorder;
class ASPlayerController;
class USBuildComponent;
class UAssetManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuildItemSelectedDelegate);

/**
 * 
 */
UCLASS(Abstract)
class ORTHOPVPSTRATEGY_API USBuildMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* MenuBorder;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UListView* BuildItemsList;

	UFUNCTION()
	void DisplayBuildMenu();
	
	UFUNCTION()
	void EndDisplayBuildMenu() const;

	FOnBuildItemSelectedDelegate OnBuildItemSelectedEvent;

protected:
	UFUNCTION()
	void OnBuildModeEnteredEvent();

	UPROPERTY()
	ASPlayerController* SPlayerController;

	UPROPERTY()
	USBuildComponent* BuildComponent;

	UPROPERTY()
	UAssetManager* AssetManager;
};
