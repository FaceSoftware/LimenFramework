// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenCameraShakeSetting.h"

#include "EngineUtils.h"
#include "Components/LimenCameraShakeComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


ULimenCameraShakeSetting::ULimenCameraShakeSetting()
{
	DevelopmentName = TEXT("setting_camerashakes");
	Category = FText::FromString(TEXT("Gameplay"));
	DisplayName = FText::FromString(TEXT("Camera Shakes"));
	Description = FText::FromString(TEXT("Whether or not to play camera shakes."));
}

void ULimenCameraShakeSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Push(EnabledString);
	PossibleSelections.Push(DisabledString);
	DefaultSelection = EnabledString;

	const FOnActorSpawned::FDelegate Delegate = FOnActorSpawned::FDelegate::CreateUObject(this,
		&ThisClass::ApplyCurrentSettingInternal);
	ApplySettingToSpawnedActorHandle = GetWorld()->AddOnActorSpawnedHandler(Delegate);
}

void ULimenCameraShakeSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	for (TActorIterator<AActor> It(GetWorld(), AActor::StaticClass(), EActorIteratorFlags::AllActors | EActorIteratorFlags::SkipPendingKill); It; ++It)
	{		
		ApplyCurrentSettingInternal(*It);
	}
}

FString ULimenCameraShakeSetting::FormatCameraShakeSetting(const bool bEnabled)
{
	return bEnabled ? EnabledString : DisabledString;
}

bool ULimenCameraShakeSetting::UnFormatCameraShakeSetting(const FString& Selection)
{
	if (Selection.Compare(EnabledString) == 0)
	{
		return true;
	}
	if (Selection.Compare(DisabledString) == 0)
	{
		return false;
	}

	checkNoEntry();
	return {};
}

void ULimenCameraShakeSetting::ApplyCurrentSettingInternal(AActor* Actor) const
{
	TArray<ULimenCameraShakeComponent*> CameraShakes;
	Actor->GetComponents<ULimenCameraShakeComponent>(CameraShakes, false);
	for (ULimenCameraShakeComponent* const& CameraShake : CameraShakes)
	{
		CameraShake->SetActive(UnFormatCameraShakeSetting(GetCurrentValue()), false);
	}
}
