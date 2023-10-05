#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

#define CUSTOM_DEPTH_RED 250
#define CUSTOM_DEPTH_GREY 251
#define CUSTOM_DEPTH_WHITE 252

UENUM(BlueprintType)
enum class EHighlightType : uint8
{
	EHT_Attack UMETA(DisplayName = "Attack"),
	EHT_Hover UMETA(DisplayName = "Hover"),
	EHT_Select UMETA(DisplayName = "Select"),


	EHT_MAX UMETA(DisplayName = "DefaultMAX")
};

UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ORTHOPVPSTRATEGY_API ISelectable
{
	GENERATED_BODY()

public:

	UFUNCTION()
	virtual void Select() = 0;

	UFUNCTION()
	virtual void DeSelect() = 0;

	UFUNCTION()
	virtual void Highlight(const bool Highlight, const EHighlightType HighlightType) = 0;
};
