#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimpleUIButtonWidget.generated.h"

class USimpleUIButtonWidget;
class UButton;
class UImage;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonClicked, USimpleUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonHovered, USimpleUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonUnHovered, USimpleUIButtonWidget*, Button, int, Index);
/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API USimpleUIButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ButtonImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonClicked OnButtonClicked;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonHovered OnButtonHovered;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FButtonUnHovered OnButtonUnHovered;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	int32 ButtonIndex;

protected:
	UFUNCTION()
	void OnSimpleUIButtonClickedEvent();

	UFUNCTION()
	void OnSimpleUIButtonHoveredEvent();

	UFUNCTION()
	void OnSimpleUIButtonUnHoveredEvent();
};
