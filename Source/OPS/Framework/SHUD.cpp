#include "SHUD.h"
#include "OPS/UI/HUDWidget.h"

ASHUD::ASHUD()
{
}


void ASHUD::CreateHUD()
{
	if (HUDClass)
	{
		HUD = CreateWidget<UHUDWidget>(GetWorld(), HUDClass);
		if (HUD != nullptr)
		{
			HUD->AddToViewport();
		}
	}
}