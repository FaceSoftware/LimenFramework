// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"
#include "Items/LimenItemBase.h"
#include "Archives/LimenArchive.h"
#include "Attributes/LimenHealthAttribute.h"
#include "Components/CapsuleComponent.h"
#include "Components/LimenArchiveComponent.h"
#include "Components/LimenCameraTiltComponent.h"
#include "Components/LimenObjectiveComponent.h"
#include "Components/LimenPhysicalItemHoldComponent.h"
#include "Components/LimenInventoryComponent.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/SpringArmComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "LogMacros/LimenPlayerLogMacros.h"
#include "PlayerControllers/LimenPlayerController.h"
#include "Managers/LimenObjective.h"
#include "PlayerStates/LimenPlayerState.h"
#include "Components/LimenCreditsComponent.h"
#include "Components/LimenDynamicDepthOfFieldComponent.h"
#include "Components/LimenCameraShakeComponent.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "Interfaces/LimenUpgradable.h"


const FName ALimenPlayerCharacter::InventoryComponentName = TEXT("InventoryComponent");
const FName ALimenPlayerCharacter::ObjectivesComponentName = TEXT("ObjectivesComponent");
const FName ALimenPlayerCharacter::SpringArmComponentName = TEXT("SpringArm");
const FName ALimenPlayerCharacter::PlayerCameraComponentName = TEXT("PlayerCamera");
const FName ALimenPlayerCharacter::WeaponHoldComponentName = TEXT("WeaponHold");
const FName ALimenPlayerCharacter::ToolHoldComponentName = TEXT("ToolHold");
const FName ALimenPlayerCharacter::CreditsComponentName = TEXT("PlayerCredits");
const FName ALimenPlayerCharacter::DynamicDepthOfFieldComponentName = TEXT("DynamicDepthOfField");
const FName ALimenPlayerCharacter::CameraShakeComponentName = TEXT("CameraShakeComponent");

ALimenPlayerCharacter::ALimenPlayerCharacter(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;
	AutoPossessAI = EAutoPossessAI::Disabled;
	
	CharacterInventory = CreateDefaultSubobject<ULimenInventoryComponent>(InventoryComponentName);
	CharacterObjectives = CreateDefaultSubobject<ULimenObjectiveComponent>(ObjectivesComponentName);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(SpringArmComponentName);
	SpringArm->SetupAttachment(GetCapsuleComponent());

	PlayerCamera = CreateDefaultSubobject<ULimenCameraTiltComponent>(PlayerCameraComponentName);
	PlayerCamera->SetupAttachment(SpringArm.Get());
	
	WeaponHold = CreateDefaultSubobject<ULimenPhysicalItemHoldComponent>(WeaponHoldComponentName);
	WeaponHold->SetupAttachment(PlayerCamera.Get());
	
	ToolHold = CreateDefaultSubobject<ULimenPhysicalItemHoldComponent>(ToolHoldComponentName);
	ToolHold->SetupAttachment(PlayerCamera.Get());

	PlayerCredits = CreateDefaultSubobject<ULimenCreditsComponent>(CreditsComponentName);

	DynamicDepthOfFieldComponent = CreateDefaultSubobject<ULimenDynamicDepthOfFieldComponent>(DynamicDepthOfFieldComponentName);

	bWasFallingLastUpdate = false;
	
	bIsAimingDownSights = false;
}

void ALimenPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterInventory->OnItemAdded.AddUniqueDynamic(this, &ThisClass::ItemAdded);
	CharacterInventory->OnItemFailedToAdd.AddUniqueDynamic(this, &ThisClass::ItemCouldNotBeAdded);
	CharacterInventory->OnItemUpdated.AddUniqueDynamic(this, &ThisClass::ItemUpdated);
	CharacterObjectives->OnNewObjectiveAdded.AddUniqueDynamic(this, &ThisClass::ObjectivesUpdated);
	
	if (GetController() && GetController()->GetClass()->IsChildOf(APlayerController::StaticClass()))
	{
		LimenPlayerControllerPtr = Cast<ALimenPlayerController>(GetController());
	}
	
	if (GetPlayerState() && GetPlayerState()->GetClass()->IsChildOf(ALimenPlayerState::StaticClass()))
	{
		LimenPlayerStatePtr = Cast<ALimenPlayerState>(GetPlayerState());
	}

	WeaponHold->OnItemChanged.AddUniqueDynamic(this, &ThisClass::WeaponChanged);
	WeaponHold->OnItemChanged.AddUniqueDynamic(this, &ThisClass::OnWeaponChanged);
	ToolHold->OnItemChanged.AddUniqueDynamic(this, &ThisClass::ToolChanged);
}

void ALimenPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ALimenPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(InteractInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::InteractInput);
	EnhancedInput->BindAction(FireInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::FireInput);
	EnhancedInput->BindAction(ReloadInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::ReloadInput);
	EnhancedInput->BindAction(ToggleToolInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::ToggleToolInput);
	EnhancedInput->BindAction(CycleToolsInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::CycleToolsInput);
	EnhancedInput->BindAction(CycleWeaponsInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ALimenPlayerCharacter::CycleWeaponsInput);
}

void ALimenPlayerCharacter::UnPossessed()
{
	LimenPlayerControllerPtr.Reset();
	LimenPlayerStatePtr.Reset();
	
	Super::UnPossessed();
}

void ALimenPlayerCharacter::HoldPhysicalItem(ALimenPhysicalItem* Item)
{
	if (Item == nullptr)
	{
		return;
	}

	if (auto* Tool = Cast<ALimenTool>(Item))
	{
		HoldTool(Tool);
	}
	else if (auto* Weapon = Cast<ALimenWeapon>(Item))
	{
		HoldWeapon(Weapon);
	}
}

void ALimenPlayerCharacter::StopHoldingPhysicalItem(const TSubclassOf<ALimenPhysicalItem>& ItemClass)
{
	if (ItemClass == nullptr)
	{
		return;
	}

	if (ItemClass->IsChildOf<ALimenTool>())
	{
		StopHoldingTool();
	}
	else if (ItemClass->IsChildOf<ALimenWeapon>())
	{
		StopHoldingWeapon();
	}
}

void ALimenPlayerCharacter::HoldTool(ALimenTool* Tool)
{
	if (Tool == nullptr)
	{
		return;
	}
	
	ToolHold->Hold(Tool);
}

void ALimenPlayerCharacter::StopHoldingTool() const
{
	ALimenTool* Tool = Cast<ALimenTool>(ToolHold->GetPhysicalItem());
	check(Tool != nullptr);
	if (Tool->IsActive())
	{
		// Auto disable tool? QoL feature...
		Tool->DeactivateTool();
	}
	ToolHold->StopHolding();
}

void ALimenPlayerCharacter::LoadHoldingTool(const TSubclassOf<ALimenTool>& ToolClass)
{
	if (ToolClass.Get() != nullptr)
	{
		ALimenTool* WeaponInstance = GetInventoryComponent()->PeekItemInstance<ALimenTool>(ToolClass);
		check(IsValid(WeaponInstance));
		HoldTool(WeaponInstance);
	}
	else
	{
		GetToolHoldComponent()->StopHolding();
	}
}

void ALimenPlayerCharacter::HoldWeapon(ALimenWeapon* Weapon)
{
	check(IsValid(Weapon))

	WeaponHold->Hold(Weapon);
}

void ALimenPlayerCharacter::StopHoldingWeapon() const
{
	WeaponHold->StopHolding();
}

void ALimenPlayerCharacter::LoadHoldingWeapon(const TSubclassOf<ALimenWeapon>& WeaponClass)
{
	if (WeaponClass.Get() != nullptr)
	{
		ALimenWeapon* WeaponInstance = GetInventoryComponent()->PeekItemInstance<ALimenWeapon>(WeaponClass);
		check(IsValid(WeaponInstance));
		HoldWeapon(WeaponInstance);
	}
	else
	{
		GetWeaponHoldComponent()->StopHolding();
	}
}

bool ALimenPlayerCharacter::IsHoldingWeapon() const
{
	return WeaponHold->IsHoldingSomething();
}

bool ALimenPlayerCharacter::IsReloading() const
{
	const ALimenWeapon* Weapon = GetCurrentWeapon();
	return Weapon == nullptr ? false : Weapon->IsReloading();
}

bool ALimenPlayerCharacter::IsFiring() const
{
	const ALimenWeapon* Weapon = GetCurrentWeapon();
	return Weapon == nullptr ? false : Weapon->IsReloading();
}

ALimenWeapon* ALimenPlayerCharacter::GetCurrentWeapon() const
{
	return Cast<ALimenWeapon>(WeaponHold->GetPhysicalItem());
}

bool ALimenPlayerCharacter::IsHoldingTool() const
{
	return ToolHold->IsHoldingSomething();
}

ALimenTool* ALimenPlayerCharacter::GetCurrentTool() const
{
	return Cast<ALimenTool>(ToolHold->GetPhysicalItem());
}

void ALimenPlayerCharacter::HandleItemActionRequests(ULimenItemAction* ActionRequested)
{
	check(ActionRequested != nullptr)
	ActionRequested->ActivateAction();
}

ULimenCameraTiltComponent* ALimenPlayerCharacter::GetPlayerCamera() const
{
	return PlayerCamera.Get();
}

ULimenInventoryComponent* ALimenPlayerCharacter::GetInventoryComponent() const
{
	return CharacterInventory.Get();
}

ULimenObjectiveComponent* ALimenPlayerCharacter::GetObjectivesComponent() const
{
	return CharacterObjectives.Get();
}

USpringArmComponent* ALimenPlayerCharacter::GetSpringArmComponent() const
{
	return SpringArm.Get();
}

ULimenCreditsComponent* ALimenPlayerCharacter::GetCreditsComponent() const
{
	return PlayerCredits.Get();
}

ULimenPhysicalItemHoldComponent* ALimenPlayerCharacter::GetToolHoldComponent() const
{
	return ToolHold.Get();
}

ULimenPhysicalItemHoldComponent* ALimenPlayerCharacter::GetWeaponHoldComponent() const
{
	return WeaponHold.Get();
}

void ALimenPlayerCharacter::GiveStartingItems(const TArray<TSubclassOf<ALimenItemBase>>& StartingItems)
{
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	for (auto& ItemClass : StartingItems)
	{
		const auto Item = GetWorld()->SpawnActor<ALimenItemBase>(ItemClass, Params);
		if (Item == nullptr)
		{
			continue;
		}

		ULimenInteractableAreaComponent* InteractableComponent = Item->GetComponentByClass<ULimenInteractableAreaComponent>();
		if (!ensureAlwaysMsgf(InteractableComponent, TEXT("Items must have a ULimenInteractableAreaComponent component")))
		{
			return;
		}
		
		InteractableComponent->Interact(GetController(), this);
		CharacterInventory->AddItem(Item);
	}
}

TArray<TScriptInterface<ILimenUpgradable>> ALimenPlayerCharacter::GetUpgradables() const
{
	TArray<ALimenItemBase*> Items = CharacterInventory->PeekItemInstancesByInterface<ULimenUpgradable>();

	TArray<TScriptInterface<ILimenUpgradable>> Output;
	Output.Reserve(Items.Num());
	
	for (auto* Item : Items)
	{
		Output.Push(TScriptInterface<ILimenUpgradable>(Item));
	}

	return Output;
}

void ALimenPlayerCharacter::SetActorHiddenInGame(const bool bNewHidden)
{
	Super::SetActorHiddenInGame(bNewHidden);

	if (ToolHold->IsHoldingSomething())
	{
		ToolHold->GetPhysicalItem()->SetActorHiddenInGame(bNewHidden);
	}
	if (IsHoldingWeapon())
	{
		WeaponHold->GetPhysicalItem()->SetActorHiddenInGame(bNewHidden);
	}
}

void ALimenPlayerCharacter::InteractInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		Interact();
	}
	else
	{
		StopInteraction();
	}
}

void ALimenPlayerCharacter::FireInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		if (!IsHoldingWeapon())
		{
			return;
		}
		
		FRotator Rotation;
		FVector Location;
		GetController()->GetPlayerViewPoint(Location, Rotation);
		GetCurrentWeapon()->StartFiring();
	}
	else
	{
		if (!IsHoldingWeapon())
		{
			return;
		}
		
		GetCurrentWeapon()->StopFiring();
	}
}

void ALimenPlayerCharacter::ReloadInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		if (!IsHoldingWeapon())
		{
			return;
		}
		
		GetCurrentWeapon()->StartReloading(GetInventoryComponent());
	}
}

void ALimenPlayerCharacter::ToggleToolInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		if (IsHoldingTool())
		{
			// Activate the tool
			check(GetCurrentTool() != nullptr);
			if (GetCurrentTool()->IsActive())
			{
				GetCurrentTool()->DeactivateTool();
			}
			else
			{
				GetCurrentTool()->ActivateTool();
			}
		}
		else if (IsHoldingWeapon())
		{
			if (bIsAimingDownSights)
			{
				StopAimingDownSights();
			}
			else
			{
				AimDownSights();
			}
		}
	}
}

void ALimenPlayerCharacter::CycleToolsInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		TArray<ALimenTool*> Tools = CharacterInventory->PeekItemInstances<ALimenTool>();
		int32 CurrentToolIndex = INDEX_NONE;
		if (GetCurrentTool() != nullptr)
		{
			for (int32 i = 0; i < Tools.Num(); i++)
			{
				if (Tools[i]->GetClass() == GetCurrentTool()->GetClass())
				{
					CurrentToolIndex = i;
					break;
				}
			}
		}
		
		if (CurrentToolIndex == INDEX_NONE)
		{
			if (!Tools.IsEmpty())
			{
				HoldTool(Tools[0]);
			}
		}
		else
		{
			if (CurrentToolIndex == Tools.Num() - 1)
			{
				StopHoldingTool();
			}
			else
			{
				check(Tools.IsValidIndex(CurrentToolIndex + 1));
				HoldTool(Tools[CurrentToolIndex + 1]);
			}
		}
	}
}

void ALimenPlayerCharacter::CycleWeaponsInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		TArray<ALimenWeapon*> Weapons = CharacterInventory->PeekItemInstances<ALimenWeapon>();
		int32 CurrentWeaponIndex = INDEX_NONE;
		if (GetCurrentWeapon() != nullptr)
		{
			if (bIsAimingDownSights)
			{
				StopAimingDownSights();
			}
			
			for (int32 i = 0; i < Weapons.Num(); i++)
			{
				if (Weapons[i]->GetClass() == GetCurrentWeapon()->GetClass())
				{
					CurrentWeaponIndex = i;
					break;
				}
			}
		}
		
		if (CurrentWeaponIndex == INDEX_NONE)
		{
			if (!Weapons.IsEmpty())
			{
				HoldWeapon(Weapons[0]);
			}
		}
		else
		{
			if (CurrentWeaponIndex == Weapons.Num() - 1)
			{
				StopHoldingWeapon();
			}
			else
			{
				check(Weapons.IsValidIndex(CurrentWeaponIndex + 1));
				HoldWeapon(Weapons[CurrentWeaponIndex + 1]);
			}
		}
	}
}

void ALimenPlayerCharacter::OnInteract(AActor* InteractableActor, const TScriptInterface<ILimenInteractableComponent>& InteractableComponent)
{
	if (InteractableComponent.GetObject() == nullptr || InteractableActor == nullptr)
	{
		return;
	}
	
	ALimenItemBase* LimenItem = Cast<ALimenItemBase>(InteractableActor);
	if (LimenItem == nullptr)
	{
		return;
	}
	
	if (CharacterInventory->AddItem(LimenItem))
	{
		LIMEN_LOG(LogLimenPlayer, Log, this, "Item picked up")
	}
}

void ALimenPlayerCharacter::ItemAdded(TSubclassOf<ALimenItemBase> NewItem)
{
	check(NewItem)
	LIMEN_LOG(LogLimenPlayer, Log, this, "Recieved new item notification")

	if (const ULimenArchiveComponent* ArchiveComponent = NewItem->GetDefaultObject<ALimenItemBase>()->GetComponentByClass<ULimenArchiveComponent>();
		ArchiveComponent != nullptr)
	{
		// Is an archive
		FNotificationParams Params;
		Params.NotificationTitle = FText::FromString(TEXT("New archive found!"));
		Params.NotificationMessage = FText::FromString(FString::Printf(TEXT("Picked up %s"), *ArchiveComponent->GetArchive()->GetDisplayName().ToString()));
		QueueNotification(Params);
	}
	else
	{
		// Is not an archive
		FNotificationParams Params;
		Params.NotificationTitle = FText::FromString(TEXT("New item"));
		Params.NotificationMessage = FText::FromString(FString::Printf(TEXT("Picked up %s"), *NewItem->GetDefaultObject<ALimenItemBase>()->GetDisplayName().ToString()));
		QueueNotification(Params);
	}
}

void ALimenPlayerCharacter::ItemCouldNotBeAdded(TSubclassOf<ALimenItemBase> NewItem)
{
	check(NewItem)
	LIMEN_LOG(LogLimenPlayer, Warning, this, "Recieved 'item could not be added' notification")
	
	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(FString::Printf(TEXT("Failed to pickup item")));
	Params.NotificationMessage = FText::FromString(FString(TEXT("Inventory full")));
	QueueNotification(Params);
}

void ALimenPlayerCharacter::ItemUpdated(TSubclassOf<ALimenItemBase> NewItem)
{
	/*
	check(NewItem)
	LIMEN_LOG(LogLimen, Warning, this, "Received 'item updated' notification")
	
	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(TEXT("New item"));
	Params.NotificationMessage = FText::FromString(FString::Printf(TEXT("Picked up %s"), *NewItem->GetDisplayName().ToString()));
	QueueNotification(Params);
	*/
}

void ALimenPlayerCharacter::ObjectivesUpdated(ALimenObjective* UpdatedObjective)
{
	check(UpdatedObjective)
	LIMEN_LOG(LogLimen, Log, this, "Recieved 'objective updated' notification")
		
	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(TEXT("Objective update"));
	Params.NotificationMessage = UpdatedObjective->GetObjectiveData().Title;
	QueueNotification(Params);
}

void ALimenPlayerCharacter::ToolChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New)
{
	if (bIsAimingDownSights)
	{
		StopAimingDownSights();
	}
	
	OnCharacterHoldTool(GetCurrentTool());
}

void ALimenPlayerCharacter::WeaponChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New)
{
	GetCurrentWeapon()->OnWeaponReload.AddUniqueDynamic(this, &ThisClass::OnWeaponReload);

	if (Old != nullptr)
	{
		ALimenWeapon* OldWeapon = Cast<ALimenWeapon>(Old);
		OldWeapon->OnWeaponReload.RemoveDynamic(this, &ThisClass::OnWeaponReload);
	}
	
	OnCharacterHoldWeapon(GetCurrentWeapon());
}

void ALimenPlayerCharacter::AimDownSights()
{
	check(!bIsAimingDownSights);

	// Aim with the weapon
	check(GetCurrentWeapon() != nullptr);
	check(GetPlayerController() != nullptr)
	GetPlayerController()->SetViewTargetWithBlend(GetCurrentWeapon(), 0.2);
	bIsAimingDownSights = true;

	OnAimDownSights(GetCurrentWeapon());
}

void ALimenPlayerCharacter::StopAimingDownSights()
{
	check(bIsAimingDownSights);
	
	// Stop aiming with the weapon
	check(GetCurrentWeapon() != nullptr);
	check(GetPlayerController() != nullptr)
	GetPlayerController()->SetViewTargetWithBlend(this, 0.2);
	bIsAimingDownSights = false;

	OnStopAimingDownSights(GetCurrentWeapon());
}
