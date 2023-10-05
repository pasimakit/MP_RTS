#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

class UFormationSelectWidget;
class ASPlayerController;
class USGameMenuWidget;
/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UFormationSelectWidget* FormationSelectionWidget;

protected:
	UFUNCTION()
	void CreateGameMenu();

	UFUNCTION()
	void SetFormationSelectionWidget(const bool bEnabled) const;

	UFUNCTION()
	void OnSelectionUpdated();

	UPROPERTY()
	USGameMenuWidget* GameMenuWidget;

	UPROPERTY()
	ASPlayerController* SPlayerController;

	UPROPERTY()
	UWorld* WorldContext;
};
