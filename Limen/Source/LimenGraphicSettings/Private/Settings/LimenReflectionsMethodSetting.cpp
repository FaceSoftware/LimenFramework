// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenReflectionsMethodSetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenReflectionsMethodSetting::ULimenReflectionsMethodSetting()
{
	DevelopmentName = TEXT("setting_reflection");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Reflections"));
	Description = FText::FromString(TEXT("Sets the method for calculating reflections. (Warning: Lumen is a lot more computationally expensive than screen space)"));
}

void ULimenReflectionsMethodSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	auto* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (auto* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenReflectionsMethodSetting::SetDefaults()
{
	PossibleSelections.Push(Lumen);
	PossibleSelections.Push(ScreenSpace);
	DefaultSelection = Lumen;
}

FString ULimenReflectionsMethodSetting::FormatReflectionSetting(const EReflectionMethod::Type Method)
{
	switch (Method)
	{
	case EDynamicGlobalIlluminationMethod::Lumen:
		return Lumen;
		
	case EDynamicGlobalIlluminationMethod::ScreenSpace:
		return ScreenSpace;
		
	default:
		checkNoEntry();
	}

	return FString();
}

EReflectionMethod::Type ULimenReflectionsMethodSetting::UnFormatReflectionSetting(const FString& Selection)
{
	if (Selection.Compare(Lumen) == 0)
	{
		return EReflectionMethod::Lumen;
	}
	if (Selection.Compare(ScreenSpace) == 0)
	{
		return EReflectionMethod::ScreenSpace;
	}

	checkNoEntry()
	return EReflectionMethod::None;
}

void ULimenReflectionsMethodSetting::GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess)
{
	GlobalPostProcess->Settings.bOverride_ReflectionMethod = true;
	GlobalPostProcess->Settings.ReflectionMethod = UnFormatReflectionSetting(GetCurrentValue());
}
