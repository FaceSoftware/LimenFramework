// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenModularSettingsListWidget.h"

#include "Components/VerticalBox.h"
#include "Engine/GameInstance.h"
#include "Settings/LimenSelectionSetting.h"
#include "Settings/DEPRECATED_LimenToggleSetting.h"
#include "Settings/LimenValueSetting.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Text/STextBlock.h"


ULimenSettingWidget::ULimenSettingWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetVisibilityInternal(ESlateVisibility::Visible);
}

void ULimenSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (SettingPtr.IsValid()) SettingBound(SettingPtr.Get());
}

void ULimenSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	if (!InSetting) return;

	check(!SettingPtr.IsValid())
	
	SettingPtr = InSetting;
	InSetting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::SettingUpdated);
	InSetting->OnSettingApplied.AddUniqueDynamic(this, &ThisClass::SettingApplied);
	InSetting->OnSettingEditableStateChanged.AddUniqueDynamic(this, &ThisClass::SettingEditableStateChanged);
	
	if (IsConstructed()) SettingBound(InSetting);
}

void ULimenValueSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	BoundSetting = CastChecked<ULimenValueSetting>(InSetting, ECastCheckedType::NullAllowed);
	Super::BindSetting(InSetting);
}

void ULimenSelectionSettingWidget::BindSetting(ULimenSetting* InSetting)
{
	BoundSetting = CastChecked<ULimenSelectionSetting>(InSetting, ECastCheckedType::NullAllowed);
	Super::BindSetting(InSetting);
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
		.Clipping(EWidgetClipping::Inherit)
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
			TSubclassOf<ULimenSettingWidget> WidgetClass = GetSettingWidget(Setting);
			if (!WidgetClass) continue;

			auto* TempWidget = NewObject<ULimenSettingWidget>(this, WidgetClass);

			// Since this does not have a widget tree,
			// we have to manually initialize the user widget
			// if (ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
			// {
			// 	TempWidget->SetPlayerContext(FLocalPlayerContext(LocalPlayer, GetWorld()));
			// }
			// TempWidget->Initialize();

			TempWidget->BindSetting(Setting);

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
	return Subsystem.IsValid();
}

void ULimenSettingsListWidget::ResolveSubsystem()
{
	if (!ModularSettingsSubsystem) return;
	const ULocalPlayer* LocalPlayer = GetOwningLocalPlayer();
	if (!LocalPlayer)
	{
		if (!GetWorld()) return;
		LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	}
	if (!LocalPlayer) { return; }
	
	const TArray<ULimenModularSettingsSubsystem*> SubsystemArray = LocalPlayer->GetSubsystemArrayCopy<ULimenModularSettingsSubsystem>();
	ULimenModularSettingsSubsystem* const* SubsystemPtr = SubsystemArray.FindByPredicate([this] (const ULimenModularSettingsSubsystem* Test)
	{
		return Test->GetClass() == ModularSettingsSubsystem.Get();
	});
	
	if (!SubsystemPtr)
	{
		Subsystem.Reset();
	}
	else
	{
		Subsystem = *SubsystemPtr;
	}
}

TSubclassOf<ULimenSettingWidget> ULimenSettingsListWidget::GetSettingWidget(const ULimenSetting* Setting)
{
	for (auto& SettingWidgetClass : SettingWidgetClasses)
	{
		if (Setting->IsA(SettingWidgetClass.Key)) return SettingWidgetClass.Value;
	}

	return nullptr;
}
