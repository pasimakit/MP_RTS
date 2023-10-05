#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FormationSelectWidget.generated.h"

class UFormationButtonWidget;
class USlider;
class USimpleUIButtonWidget;
class ASPlayerController;
/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API UFormationSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* LineButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* ColumnButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* WedgeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationButtonWidget* BlobButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* SpacingSlider;

protected:

	UFUNCTION()
	void OnFormationButtonClicked(USimpleUIButtonWidget* Button, int Index);

	UFUNCTION()
	void OnSpacingValueChanged(const float Value);

	UPROPERTY()
	ASPlayerController* SPlayerController;
};
