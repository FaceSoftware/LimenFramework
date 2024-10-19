// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenNvidiaReflexSetting.h"

#include "StreamlineLibraryReflex.h"


ULimenNvidiaReflexSetting::ULimenNvidiaReflexSetting()
{
	DevelopmentName = TEXT("setting_nvidiareflex");
	Category = FText::FromString(TEXT("Nvidia"));
	DisplayName = FText::FromString(TEXT("Reflex"));
	Description = FText::FromString(TEXT("Nvidia reflex mode"));
}

bool ULimenNvidiaReflexSetting::CanEdit() const
{
	if (!Super::CanEdit())
	{
		return false;
	}

	return UStreamlineLibraryReflex::IsReflexSupported();
}

void ULimenNvidiaReflexSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	const UStreamlineReflexMode Mode = UnFormatNvidiaReflexSetting(GetCurrentValue());
	if (CanEdit())
	{
		UStreamlineLibraryReflex::SetReflexMode(Mode);
	}
}

void ULimenNvidiaReflexSetting::SetDefaults()
{
	PossibleSelections.Push(Disabled);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(EnabledPlusBoost);
	DefaultSelection = FormatNvidiaReflexSetting(UStreamlineReflexMode::Disabled);
}

FString ULimenNvidiaReflexSetting::FormatNvidiaReflexSetting(const UStreamlineReflexMode Mode)
{
	switch (Mode)
	{
	case UStreamlineReflexMode::Disabled:
		return Disabled;
	case UStreamlineReflexMode::Enabled:
		return Enabled;
	case UStreamlineReflexMode::EnabledPlusBoost:
		return EnabledPlusBoost;
	}

	checkNoEntry();
	return {};
}

UStreamlineReflexMode ULimenNvidiaReflexSetting::UnFormatNvidiaReflexSetting(const FString& Selection)
{
	if (Selection.Compare(Disabled) == 0)
	{
		return UStreamlineReflexMode::Disabled;
	}
	if (Selection.Compare(Enabled) == 0)
	{
		return UStreamlineReflexMode::Enabled;
	}
	if (Selection.Compare(EnabledPlusBoost) == 0)
	{
		return UStreamlineReflexMode::EnabledPlusBoost;
	}

	checkNoEntry();
	return {};
}
