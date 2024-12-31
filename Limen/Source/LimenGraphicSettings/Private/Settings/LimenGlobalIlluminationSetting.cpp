// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenGlobalIlluminationSetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenGlobalIlluminationSetting::ULimenGlobalIlluminationSetting()
{
	DevelopmentName = TEXT("setting_gi");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Global Illumination"));
	Description = FText::FromString(TEXT("Sets the method for calculating global illumination. (Warning: Lumen is a lot more computationally expensive than screen space)"));
}

void ULimenGlobalIlluminationSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();
	
	ULimenGraphicalSettingsSubsystem* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (APostProcessVolume* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenGlobalIlluminationSetting::SetDefaults()
{
	PossibleSelections.Push(Lumen);
	PossibleSelections.Push(ScreenSpace);
	DefaultSelection = Lumen;
}

FString ULimenGlobalIlluminationSetting::FormatGISetting(const EDynamicGlobalIlluminationMethod::Type Method)
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

EDynamicGlobalIlluminationMethod::Type ULimenGlobalIlluminationSetting::UnFormatGISetting(const FString& Selection)
{
	if (Selection.Compare(Lumen) == 0)
	{
		return EDynamicGlobalIlluminationMethod::Lumen;
	}
	if (Selection.Compare(ScreenSpace) == 0)
	{
		return EDynamicGlobalIlluminationMethod::ScreenSpace;
	}

	checkNoEntry()
	return EDynamicGlobalIlluminationMethod::None;
}

void ULimenGlobalIlluminationSetting::GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess)
{
	GlobalPostProcess->Settings.bOverride_DynamicGlobalIlluminationMethod = true;
	GlobalPostProcess->Settings.DynamicGlobalIlluminationMethod = UnFormatGISetting(GetCurrentValue());
}
