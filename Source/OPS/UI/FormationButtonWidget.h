#pragma once

#include "CoreMinimal.h"
#include "SimpleUIButtonWidget.h"
#include "OPS/AI/AIData.h"
#include "FormationButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class ORTHOPVPSTRATEGY_API UFormationButtonWidget : public USimpleUIButtonWidget
{
	GENERATED_BODY()
public:
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<EFormation> Formation;
};
