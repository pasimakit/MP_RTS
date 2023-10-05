#pragma once

#include "CoreMinimal.h"
#include "InputTriggers.h"
#include "InputTriggerDoubleTap.generated.h"

/**
 * 
 */
UCLASS(NotBluePrintable, MinimalAPI, meta = (DisplayName = "Double Tap", NotInputConfigurable = "true"))
class UInputTriggerDoubleTap : public UInputTrigger
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	float Delay = 0.5f;

protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;

private:
	float LastTappedTime = 0.f;
};