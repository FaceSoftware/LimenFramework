// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenMouseSensitivityComponent.h"


void ULimenMouseSensitivityComponent::SetMouseParameters(const FMouseParameters& InNewParams)
{
	MouseParameters = InNewParams;

	OnSensitivityUpdated.Broadcast(this);
}

void ULimenMouseSensitivityComponent::SetSensitivity(const float Sensitivity)
{
	MouseParameters.MouseX.Value = Sensitivity;
	MouseParameters.MouseY.Value = Sensitivity;

	OnSensitivityUpdated.Broadcast(this);
}

void ULimenMouseSensitivityComponent::SetInvertAxis(const bool bInvertX, const bool bInvertY)
{
	MouseParameters.MouseX.Key = bInvertX;
	MouseParameters.MouseY.Key = bInvertY;

	OnSensitivityUpdated.Broadcast(this);
}

float ULimenMouseSensitivityComponent::GetMouseX() const
{
	return MouseParameters.MouseX.Value;
}

bool ULimenMouseSensitivityComponent::GetInvertMouseX() const
{
	return MouseParameters.MouseX.Key;
}

float ULimenMouseSensitivityComponent::GetMouseY() const
{
	return MouseParameters.MouseY.Value;
}

bool ULimenMouseSensitivityComponent::GetInvertMouseY() const
{
	return MouseParameters.MouseY.Key;
}

float ULimenMouseSensitivityComponent::GetMouseXY() const
{
	if (!FMath::IsNearlyEqual(MouseParameters.MouseX.Value, MouseParameters.MouseY.Value))
	{
		return NAN;
	}

	return MouseParameters.MouseX.Value;
}

const FMouseParameters& ULimenMouseSensitivityComponent::GetMouseParameters() const
{
	return MouseParameters;
}
