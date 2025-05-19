// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenModularSettingsListWidget.h"

#include "Components/VerticalBox.h"
#include "Engine/GameInstance.h"
#include "Settings/LimenSelectionSetting.h"
#include "Settings/LimenToggleSetting.h"
#include "Settings/LimenValueSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Text/STextBlock.h"


void ULimenValueSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BoundSetting.IsValid()) SettingBound(BoundSetting.Get());
}

void ULimenValueSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	BoundSetting = CastChecked<ULimenValueSetting>(InSetting, ECastCheckedType::NullAllowed);
	if (IsConstructed()) SettingBound(BoundSetting.Get());
}

void ULimenSelectionSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BoundSetting.IsValid()) SettingBound(BoundSetting.Get());
}

void ULimenSelectionSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	BoundSetting = CastChecked<ULimenSelectionSetting>(InSetting, ECastCheckedType::NullAllowed);
	if (IsConstructed()) SettingBound(BoundSetting.Get());
}

void ULimenToggleSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (BoundSetting.IsValid()) SettingBound(BoundSetting.Get());
}

void ULimenToggleSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	BoundSetting = CastChecked<ULimenToggleSetting>(InSetting, ECastCheckedType::NullAllowed);
	if (IsConstructed()) SettingBound(BoundSetting.Get());
}

ULimenSettingsListWidget::ULimenSettingsListWidget()
{
	SettingVerticalAlignment = VAlign_Top;
	SettingHorizontalAlignment = HAlign_Fill;
	CategoryVerticalAlignment = VAlign_Top;
	CategoryHorizontalAlignment = HAlign_Fill;
	CategoryTextJustification = ETextJustify::Left;
}

TSharedRef<SWidget> ULimenSettingsListWidget::RebuildWidget()
{
	ResolveSubsystem();
	
	const TSharedRef<SScrollBox> SettingsContainer = SNew(SScrollBox)
		.Clipping(EWidgetClipping::ClipToBounds)
		.Orientation(Orient_Vertical);

	if (!AreParamsValid()) return SettingsContainer;

	for (const auto& Category : Subsystem->GetCategoriesInAlphabeticalOrder())
	{
		const TSharedRef<SVerticalBox> CategoryContainer = SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.SizeParam(FAuto())
			[
				SNew(STextBlock)
					.Text(Category)
					.ColorAndOpacity(CategoryTextColor)
					.Font(CategoryFont)
					.Justification(CategoryTextJustification)
					.Margin(CategoryTextMargin)
			];

		for (auto* Setting : Subsystem->GetAllItemsOfCategory<ULimenSetting>(Category))
		{
			UClass* WidgetClass = nullptr;
			auto* ValueTest = Cast<ULimenValueSetting>(Setting);
			auto* SelectionTest = Cast<ULimenSelectionSetting>(Setting);
			auto* ToggleSTest = Cast<ULimenToggleSetting>(Setting);

			if (ValueTest) WidgetClass = ValueSettingWidgetClass;
			else if (SelectionTest) WidgetClass = SelectionSettingWidgetClass;
			else if (ToggleSTest) WidgetClass = ToggleSettingWidgetClass;

			check(WidgetClass != nullptr)
			auto* TempWidget = NewObject<ULimenSettingWidget>(this, WidgetClass);
			if (!TempWidget) continue;

			// Since this does not have a widget tree,
			// we have to manually initialize the user widget
			if (GetOwningLocalPlayer())
			{
				TempWidget->SetPlayerContext(FLocalPlayerContext(GetOwningLocalPlayer(), GetWorld()));
			}
			TempWidget->Initialize();

			if (ValueTest) TempWidget->BindSetting(ValueTest);
			else if (SelectionTest) TempWidget->BindSetting(SelectionTest);
			else if (ToggleSTest) TempWidget->BindSetting(ToggleSTest);

			CategoryContainer->AddSlot()
				.VAlign(SettingVerticalAlignment)
				.HAlign(SettingHorizontalAlignment)
				.SizeParam(FAuto())
				.AttachWidget(TempWidget->TakeWidget());
		}

		SettingsContainer->AddSlot()
			.VAlign(CategoryVerticalAlignment)
			.HAlign(CategoryHorizontalAlignment)
			.SizeParam(FAuto())
			.AttachWidget(CategoryContainer);
	}

	return SettingsContainer;
}

bool ULimenSettingsListWidget::AreParamsValid() const
{
	return ValueSettingWidgetClass		!= nullptr &&
		   SelectionSettingWidgetClass	!= nullptr &&
		   Subsystem.IsValid();
}

void ULimenSettingsListWidget::ResolveSubsystem()
{
	if (!ModularSettingsSubsystem) return;
	if (!GetWorld()) return;
	if (!GetWorld()->GetGameInstance()) return;

	const auto SubsystemBasePtr = GetWorld()->GetGameInstance()->GetSubsystemBase(ModularSettingsSubsystem);
	Subsystem = CastChecked<ULimenModularSettingsSubsystem>(SubsystemBasePtr, ECastCheckedType::NullAllowed);
}
