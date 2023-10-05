#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "SHUD.generated.h"

class UHUDWidget;
/**
 * 
 */
UCLASS(Abstract)
class ORTHOPVPSTRATEGY_API ASHUD : public AHUD
{
	GENERATED_BODY()

public:
	ASHUD();

	UFUNCTION()
	void CreateHUD();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|UI")
	TSubclassOf<UUserWidget> GameMenuWidgetClass;

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|UI")
	TSubclassOf<UUserWidget> HUDClass;


	UPROPERTY()
	UHUDWidget* HUD;

protected:
};
