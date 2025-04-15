// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Abilities/LimenVariableMovementAbility.h"
#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"
#include "Items/LimenItemBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/LimenAbilityComponent.h"
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
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUDs/LimenHUD.h"
#include "Interfaces/LimenUpgradable.h"
#include "MappableKeySettings/LimenPlayerMappableKeySettings.h"
#include "Subsystems/LimenKeyBindSubsystem.h"


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
	
	VariableMovementAbility = GetAbilityComponent()->GetAbility<ULimenVariableMovementAbility>();
	LimenPlayerControllerPtr = Cast<ALimenPlayerController>(GetController());
	LimenPlayerStatePtr = Cast<ALimenPlayerState>(GetPlayerState());

	CharacterInventory->OnItemAdded.AddUniqueDynamic(this, &ThisClass::ItemAdded);
	CharacterInventory->OnItemFailedToAdd.AddUniqueDynamic(this, &ThisClass::ItemCouldNotBeAdded);
	CharacterInventory->OnItemUpdated.AddUniqueDynamic(this, &ThisClass::ItemUpdated);

	CharacterObjectives->OnNewObjectiveAdded.AddUniqueDynamic(this, &ThisClass::ObjectiveAdded);
	CharacterObjectives->OnObjectiveUpdated.AddUniqueDynamic(this, &ThisClass::ObjectiveUpdated);
	CharacterObjectives->OnObjectiveCompleted.AddUniqueDynamic(this, &ThisClass::ObjectiveComplete);
	
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

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (InputSystem != nullptr && KeyBindSubsystem != nullptr)
	{
		if (const UInputMappingContext* PlayerMappings = KeyBindSubsystem->GetPawnInputMappingContext(this);
			PlayerMappings != nullptr)
		{
			InputSystem->AddMappingContext(PlayerMappings, 1);
			for (auto& Mapping : PlayerMappings->GetMappings())
			{
				InputBindUpdated(Mapping);
			}
			
			KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ThisClass::InputBindUpdated);
		}
	}
	
	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EnhancedInput->BindAction(MoveForwardInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveForwardInput);
	EnhancedInput->BindAction(MoveLeftInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveLeftInput);
	EnhancedInput->BindAction(MoveBackwardsInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveBackwardsInput);
	EnhancedInput->BindAction(MoveRightInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::MoveRightInput);
	EnhancedInput->BindAction(LookInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::LookInput);
	EnhancedInput->BindAction(SprintInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::SprintInput);
	EnhancedInput->BindAction(JumpInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::JumpInput);
	EnhancedInput->BindAction(CrouchInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::CrouchInput);
	EnhancedInput->BindAction(GameMenuInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::GameMenuInput);
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

UCameraComponent* ALimenPlayerCharacter::GetPlayerCamera() const
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

		CharacterInventory->AddItem(Item);
		Item->PickUp(GetController(), this);
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

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetMoveForwardInputAction() const
{
	return MoveForwardInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetMoveLeftInputAction() const
{
	return MoveLeftInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetMoveBackwardsInputAction() const
{
	return MoveBackwardsInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetMoveRightInputAction() const
{
	return MoveRightInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetLookInputAction() const
{
	return LookInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetSprintInputAction() const
{
	return SprintInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetJumpInputAction() const
{
	return JumpInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenPlayerCharacter::GetCrouchInputAction() const
{
	return CrouchInputAction;
}

void ALimenPlayerCharacter::InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetPlayerController()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	FModifyContextOptions ContextOptions;
	ContextOptions.bForceImmediately = false;
	ContextOptions.bNotifyUserSettings = false;
	ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

	InputSystem->RequestRebuildControlMappings(ContextOptions, EInputMappingRebuildType::RebuildWithFlush);

	const auto* Settings = Cast<ULimenPlayerMappableKeySettings>(ActionKeyMapping.GetPlayerMappableKeySettings());
	if (Settings == nullptr)
	{
		return;
	}
	
	if (ActionKeyMapping.Action == SprintInputAction)
	{
		SprintHandlingType = Settings->InputHandlingType;
	}
	else if (ActionKeyMapping.Action == CrouchInputAction)
	{
		CrouchHandlingType = Settings->InputHandlingType;
	}
}

void ALimenPlayerCharacter::GameMenuInput(const FInputActionInstance& Instance)
{
	ALimenHUD* Hud = GetPlayerController()->GetHUD<ALimenHUD>();
	if (Hud == nullptr)
	{
		return;
	}
	
	if (Instance.GetValue().Get<bool>())
	{
		if (Hud->IsCharacterHudShowing())
		{
			Hud->ToggleGameMenuWidget();
		}
		else // Is not seeing the hud, probably inside a menu
		{
			Hud->HideActiveWidget();
		}
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

inline void ALimenPlayerCharacter::MoveForwardInput(const FInputActionInstance& Instance)
{
	OnMovementInput.Broadcast();
	const auto Input = Instance.GetValue().Get<float>();
	if (!FMath::IsNearlyZero(Input))
	{
		AddMovementInput(GetActorForwardVector(), Input);
	}
}

inline void ALimenPlayerCharacter::MoveLeftInput(const FInputActionInstance& Instance)
{
	OnMovementInput.Broadcast();
	const auto Input = Instance.GetValue().Get<float>();
	if (!FMath::IsNearlyZero(Input))
	{
		AddMovementInput(GetActorRightVector(), -Input);
	}
}

inline void ALimenPlayerCharacter::MoveBackwardsInput(const FInputActionInstance& Instance)
{
	OnMovementInput.Broadcast();
	const auto Input = Instance.GetValue().Get<float>();
	if (!FMath::IsNearlyZero(Input))
	{
		AddMovementInput(GetActorForwardVector(), -Input);
	}
}

inline void ALimenPlayerCharacter::MoveRightInput(const FInputActionInstance& Instance)
{
	OnMovementInput.Broadcast();
	const auto Input = Instance.GetValue().Get<float>();
	if (!FMath::IsNearlyZero(Input))
	{
		AddMovementInput(GetActorRightVector(), Input);
	}
}

void ALimenPlayerCharacter::LookInput(const FInputActionInstance& Instance)
{
	const auto Input = Instance.GetValue().Get<FVector2D>();
	AddControllerYawInput(Input.X);
	AddControllerPitchInput(-Input.Y);
}

void ALimenPlayerCharacter::SprintInput(const FInputActionInstance& Instance)
{
	bool bValue = Instance.GetValue().Get<bool>();

	switch (SprintHandlingType)
	{
	case EInputActionHandlingType::Hold:
		bValue ? StartSprinting() : StopSprinting();
		break;
		
	case EInputActionHandlingType::Toggle:
		if (bValue) VariableMovementAbility->IsActive() ? StopSprinting() : StartSprinting();
		break;

	default:
		break;
	}
	
	OnSprintInput.Broadcast();
}

void ALimenPlayerCharacter::JumpInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ALimenPlayerCharacter::CrouchInput(const FInputActionInstance& Instance)
{
	bool bValue = Instance.GetValue().Get<bool>();

	switch (CrouchHandlingType)
	{
	case EInputActionHandlingType::Hold:
		bValue ? Crouch() : UnCrouch();
		break;
		
	case EInputActionHandlingType::Toggle:
		if (bValue) GetCharacterMovement()->bWantsToCrouch ? UnCrouch() : Crouch();
		break;

	default:
		break;
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
		LimenItem->PickUp(GetController(), this);
		LIMEN_LOG(LogLimenPlayer, Log, this, "Item picked up")
	}
}

void ALimenPlayerCharacter::ItemAdded(TSubclassOf<ALimenItemBase> NewItem)
{
	check(NewItem != nullptr)
	LIMEN_LOG(LogLimenPlayer, Log, this, "Received new item notification")
	
	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(TEXT("New item"));
	Params.NotificationMessage = FText::FromString(FString::Printf(TEXT("Picked up %s"), *NewItem->GetDefaultObject<ALimenItemBase>()->GetDisplayName().ToString()));
	QueueNotification(Params);
}

void ALimenPlayerCharacter::ItemCouldNotBeAdded(TSubclassOf<ALimenItemBase> NewItem)
{
	check(NewItem != nullptr)
	LIMEN_LOG(LogLimenPlayer, Warning, this, "Received 'item could not be added' notification")
	
	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(FString::Printf(TEXT("Failed to pickup item")));
	Params.NotificationMessage = FText::FromString(FString(TEXT("Inventory full")));
	QueueNotification(Params);
}

void ALimenPlayerCharacter::ItemUpdated(TSubclassOf<ALimenItemBase> NewItem)
{
	check(NewItem != nullptr)
	LIMEN_LOG(LogLimen, Warning, this, "Received 'item updated' notification")

	FNotificationParams Params;
	Params.NotificationTitle = FText::FromString(TEXT("New item"));
	Params.NotificationMessage = FText::FromString(FString::Printf(TEXT("Picked up %s"), *NewItem->GetDefaultObject<ALimenItemBase>()->GetDisplayName().ToString()));
	QueueNotification(Params);
}

void ALimenPlayerCharacter::ObjectiveAdded(ALimenObjective* NewObjective)
{
	check(NewObjective != nullptr)

	const FString Message = TEXT("New Objective: ") + NewObjective->GetObjectiveData().Title.ToString();
	
	FNotificationParams Params;
	Params.NotificationMessage = FText::FromString(Message);
	QueueNotification(Params);

	LIMEN_LOG(LogLimen, Log, this, "Received 'new objective' notification")
}

void ALimenPlayerCharacter::ObjectiveUpdated(ALimenObjective* UpdatedObjective)
{
	check(UpdatedObjective != nullptr)
	
	const FString Message = TEXT("Objective Updated: ") + UpdatedObjective->GetObjectiveData().Title.ToString();
	
	FNotificationParams Params;	
	Params.NotificationMessage = FText::FromString(Message);
	QueueNotification(Params);

	LIMEN_LOG(LogLimen, Log, this, "Received 'objective updated' notification")
}

void ALimenPlayerCharacter::ObjectiveComplete(ALimenObjective* UpdatedObjective)
{
	check(UpdatedObjective != nullptr)

	const FString Message = TEXT("Objective Completed: ") + UpdatedObjective->GetObjectiveData().Title.ToString();

	FNotificationParams Params;
	Params.NotificationMessage = FText::FromString(Message);
	QueueNotification(Params);

	LIMEN_LOG(LogLimen, Log, this, "Received 'objective completed' notification")
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

void ALimenPlayerCharacter::StartSprinting()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}

	if (VariableMovementAbility->CanActivateAbility())
	{
		VariableMovementAbility->ActivateAbility(GetController(), this);
	}
}

void ALimenPlayerCharacter::StopSprinting()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}
	
	VariableMovementAbility->CancelAbility(GetController(), this);
}

void ALimenPlayerCharacter::ToggleSprint()
{
	if (VariableMovementAbility == nullptr)
	{
		return;
	}
	
	if (VariableMovementAbility->IsActive())
	{
		StartSprinting();
	}
	else
	{
		StopSprinting();
	}
}
