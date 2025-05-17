// Copyright Face Software. All Rights Reserved.


#include "UMG/Buttons/LimenMenuButton.h"

#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Slate/SRetainerWidget.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"


ULimenMenuButton::ULimenMenuButton() : Super(), bUseIcon(true)
{
	TextHorizontalAlignment = HAlign_Left;
	TextVerticalAlignment = VAlign_Center;
	TextureParameter = TEXT("Texture");
	ToggleParameter = TEXT("Toggle");

	EffectDurationSeconds = 0.1f;
	HoverEffectElapsedTime = 0.f;

	bIsPlayingDistortionEffect = false;
}

TSharedRef<SWidget> ULimenMenuButton::RebuildWidget()
{
	if (bUseIcon)
	{
		Icon = SNew(SImage)
			.Image(&IconBrush)
			.Visibility(IsDesignTime() ? EVisibility::Visible : EVisibility::Hidden);
	}

	TextBlock = SNew(STextBlock)
				 .Text(Text)
				 .ColorAndOpacity(TextColor)
				 .Font(Font)
				 .Justification(TextJustification);

	if (bUseIcon)
	{
		RetainerBox = SNew(SRetainerWidget)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Left)
			.VAlign(VAlign_Center)
			.SizeParam(FStretch())
			.Padding(IconMargin)
			[
				Icon.ToSharedRef()
			]
					
			+ SHorizontalBox::Slot()
			.HAlign(TextHorizontalAlignment)
			.VAlign(TextVerticalAlignment)
			.SizeParam(FStretch())
			.Padding(TextMargin)
			[
				TextBlock.ToSharedRef()
			]
		];
	}
	else
	{
		RetainerBox = SNew(SRetainerWidget)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.HAlign(TextHorizontalAlignment)
			.VAlign(TextVerticalAlignment)
			.SizeParam(FStretch())
			.Padding(TextMargin)
			[
				TextBlock.ToSharedRef()
			]
		];
	}
	
	TSharedRef<SWidget> Root = SNew(SOverlay)
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		RetainerBox.ToSharedRef()
	]
	
	+ SOverlay::Slot()
	.HAlign(HAlign_Fill)
	.VAlign(VAlign_Fill)
	[
		SNew(SButton)
		.ButtonStyle(&ButtonStyle)
		.OnClicked(BIND_UOBJECT_DELEGATE(FOnClicked, OnClicked))
		.OnHovered(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnHovered))
		.OnUnhovered(BIND_UOBJECT_DELEGATE(FSimpleDelegate, OnUnhovered))
	];
	
	RetainerBox->SetTextureParameter(TextureParameter);
	RetainerBox->SetEffectMaterial(EffectMaterial.Get());
	
	StopDistortionEffect();
	
	return Root;
}

void ULimenMenuButton::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	Icon.Reset();
	TextBlock.Reset();
	RetainerBox.Reset();
}

FReply ULimenMenuButton::OnClicked()
{
	PlayDistortionEffect();
	UGameplayStatics::PlaySound2D(this, ClickSound.Get());

	return Super::OnClicked();
}

void ULimenMenuButton::OnHovered()
{
	Super::OnHovered();

	if (Icon.IsValid()) Icon->SetVisibility(EVisibility::Visible);

	if (PlayDistortionEffect())
	{
		UGameplayStatics::PlaySound2D(this, HoverSound.Get());
	}
}

void ULimenMenuButton::OnUnhovered()
{
	Super::OnUnhovered();
	
	if (Icon.IsValid()) Icon->SetVisibility(EVisibility::Hidden);

	if (PlayDistortionEffect())
	{
		UGameplayStatics::PlaySound2D(this, UnhoverSound.Get());
	}
}

TSharedRef<SImage> ULimenMenuButton::GetIcon() const
{
	return Icon.ToSharedRef();
}

bool ULimenMenuButton::PlayDistortionEffect()
{
	if (RetainerBox->GetEffectMaterial() == nullptr)
	{
		return false;
	}
	
	RetainerBox->GetEffectMaterial()->SetScalarParameterValue(ToggleParameter, 1.f);

	HoverEffectElapsedTime = 0;
	bIsPlayingDistortionEffect = true;
	
	return true;
}

void ULimenMenuButton::StopDistortionEffect()
{
	if (RetainerBox.Get() != nullptr && RetainerBox->GetEffectMaterial() != nullptr)
	{
		RetainerBox->GetEffectMaterial()->SetScalarParameterValue(ToggleParameter, 0.f);
	}

	bIsPlayingDistortionEffect = false;
}

void ULimenMenuButton::Tick(float DeltaTime)
{
	if (bIsPlayingDistortionEffect)
	{
		HoverEffectElapsedTime += DeltaTime;
	}
	
	if (HoverEffectElapsedTime > EffectDurationSeconds)
	{
		HoverEffectElapsedTime = 0;
		StopDistortionEffect();
	}
}

ETickableTickType ULimenMenuButton::GetTickableTickType() const
{
	return ETickableTickType::Always;
}

bool ULimenMenuButton::IsTickable() const
{
	return true;
}

bool ULimenMenuButton::IsAllowedToTick() const
{
	return true;
}

TStatId ULimenMenuButton::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenMenuButton, STATGROUP_Tickables);
}

bool ULimenMenuButton::IsTickableWhenPaused() const
{
	return true;
}

bool ULimenMenuButton::IsTickableInEditor() const
{
	return false;
}

UWorld* ULimenMenuButton::GetTickableGameObjectWorld() const
{
	return GetWorld();
}
