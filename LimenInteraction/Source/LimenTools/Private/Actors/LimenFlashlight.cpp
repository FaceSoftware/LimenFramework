// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenFlashlight.h"

#include "Components/LimenConeTraceComponent.h"
#include "Components/SpotLightComponent.h"


ALimenFlashlight::ALimenFlashlight()
{
	PrimaryActorTick.bCanEverTick = true;

	Light = CreateDefaultSubobject<USpotLightComponent>(TEXT("Light"));
	Light->SetupAttachment(GetRootComponent());
	check(Light);

	ConeTrace = CreateDefaultSubobject<ULimenConeTraceComponent>(TEXT("ConeTrace"));
	check(ConeTrace);
}

void ALimenFlashlight::BeginPlay()
{
	Super::BeginPlay();
	
	ConeTrace->OnConeCollisionUpdate.AddDynamic(this, &ThisClass::OnActorLitUpdate);
}

void ALimenFlashlight::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	const FConeData ConeData = FConeData::FromAngle(Light->GetComponentLocation(),
													Light->GetComponentQuat(),
													Light->OuterConeAngle * 2,
													Light->AttenuationRadius);

	ConeTrace->SetConeData(ConeData);
}

ULimenConeTraceComponent* ALimenFlashlight::GetConeTraceComponent() const
{
	return ConeTrace.Get();
}

USpotLightComponent* ALimenFlashlight::GetLight() const
{
	return Light.Get();
}

void ALimenFlashlight::ToolActivated()
{
	Super::ToolActivated();

	Light->SetVisibility(true, false);
	ConeTrace->Activate(false);
}

void ALimenFlashlight::ToolDeactivated()
{
	Super::ToolDeactivated();

	Light->SetVisibility(false, false);
	ConeTrace->Deactivate();
}
