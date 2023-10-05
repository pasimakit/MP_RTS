#include "HUDWidget.h"
#include "Kismet/GameplayStatics.h"
#include "OPS/Controller/SPlayerController.h"
#include "OPS/UI/FormationSelectWidget.h"
#include "OPS/Framework/SHUD.h"
#include "Build/SGameMenuWidget.h"

void UHUDWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((WorldContext = GetWorld()) != nullptr);
	verify((SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(WorldContext, 0))) != nullptr);

	SetFormationSelectionWidget(false);

	if (SPlayerController)
	{
		SPlayerController->OnSelectedUpdated.AddDynamic(this, &UHUDWidget::OnSelectionUpdated);
	}

	CreateGameMenu();
}

void UHUDWidget::CreateGameMenu()
{
	if (!SPlayerController || !WorldContext) return;

	if (const ASHUD* SHUD = Cast<ASHUD>(SPlayerController->GetHUD()))
	{
		if (SHUD->GameMenuWidgetClass)
		{
			GameMenuWidget = Cast<USGameMenuWidget>(CreateWidget<UUserWidget>(WorldContext, SHUD->GameMenuWidgetClass));
			if (GameMenuWidget != nullptr)
			{
				GameMenuWidget->AddToViewport();
			}
		}
	}
}

void UHUDWidget::SetFormationSelectionWidget(const bool bEnabled) const
{
	if (FormationSelectionWidget)
	{
		FormationSelectionWidget->SetVisibility(bEnabled ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UHUDWidget::OnSelectionUpdated()
{
	if (SPlayerController)
	{
		SetFormationSelectionWidget(SPlayerController->HasGroupSelection());
	}
}
