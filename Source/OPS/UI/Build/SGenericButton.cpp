// Fill out your copyright notice in the Description page of Project Settings.


#include "SGenericButton.h"
#include "Components/TextBlock.h"

void USGenericButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (ButtonTextBlock)
	{
		ButtonTextBlock->SetText(FText::FromString(ButtonText));
	}
}
