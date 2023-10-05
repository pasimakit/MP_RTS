#include "FormationSelectWidget.h"
#include "OPS/UI/FormationButtonWidget.h"
#include "OPS/Controller/SPlayerController.h"
#include "Components/Slider.h"

void UFormationSelectWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	verify((SPlayerController = Cast<ASPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0))) != nullptr);

	if (LineButton)
	{
		LineButton->OnButtonClicked.AddDynamic(this, &UFormationSelectWidget::OnFormationButtonClicked);

		// Default formation

		OnFormationButtonClicked(LineButton, 0);
	}

	if (ColumnButton)
	{
		ColumnButton->OnButtonClicked.AddDynamic(this, &UFormationSelectWidget::OnFormationButtonClicked);
	}

	if (WedgeButton)
	{
		WedgeButton->OnButtonClicked.AddDynamic(this, &UFormationSelectWidget::OnFormationButtonClicked);
	}

	if (BlobButton)
	{
		BlobButton->OnButtonClicked.AddDynamic(this, &UFormationSelectWidget::OnFormationButtonClicked);
	}

	if (SpacingSlider)
	{
		SpacingSlider->OnValueChanged.AddDynamic(this, &UFormationSelectWidget::OnSpacingValueChanged);

		// Default spacing
		OnSpacingValueChanged(SpacingSlider->GetValue());
	}
}

void UFormationSelectWidget::OnFormationButtonClicked(USimpleUIButtonWidget* Button, int Index)
{
	if (SPlayerController)
	{
		SPlayerController->UpdateFormation(static_cast<EFormation>(Index));
	}
}

void UFormationSelectWidget::OnSpacingValueChanged(const float Value)
{
	if (SPlayerController)
	{
		SPlayerController->UpdateSpacing(Value);
	}
}
